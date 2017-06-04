//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "RenderProcessHandler.h"
#include "include/base/cef_logging.h"
#include "include/wrapper/cef_helpers.h"
#include <sstream>

#include "src/CEF/Data/DOMExtraction.h"
#include "src/CEF/Data/DOMNode.h"		// TODO: Move descriptions to DOMExtraction to not need to include this header?

RenderProcessHandler::RenderProcessHandler()
{
	CefMessageRouterConfig config;
	config.js_query_function = "cefQuery";
	config.js_cancel_function = "cefQueryCancel";
	_msgRouter = CefMessageRouterRendererSide::Create(config);
}

CefRefPtr<CefV8Value> RenderProcessHandler::CefValueToCefV8Value(CefRefPtr<CefValue> val)
{
	switch (val->GetType()) {
	case VTYPE_BOOL:		return CefV8Value::CreateBool(val->GetBool());
	case VTYPE_INT:			return CefV8Value::CreateInt(val->GetInt());
	case VTYPE_DOUBLE:		return CefV8Value::CreateDouble(val->GetDouble());
	case VTYPE_STRING:		return CefV8Value::CreateString(val->GetString());
	case VTYPE_LIST:
	{
		const auto& size = val->GetList()->GetSize();
		CefRefPtr<CefListValue> list = val->GetList();
		CefRefPtr<CefV8Value> v8list = CefV8Value::CreateArray(size);
		for (int i = 0; i < size; i++)
		{
			v8list->SetValue(i, CefValueToCefV8Value(list->GetValue(i)));
		}
		return v8list;
	}
	default:				return CefV8Value::CreateUndefined();
	}
}


bool RenderProcessHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId sourceProcess,
    CefRefPtr<CefProcessMessage> msg)
{
    CEF_REQUIRE_RENDERER_THREAD();

    const std::string& msgName = msg->GetName().ToString();
    //IPCLogDebug(browser, "Received '" + msgName + "' IPC msg in RenderProcessHandler");

	if (msgName == "ExecuteJavascriptFunction")
	{
		const auto& args = msg->GetArgumentList();
		const std::string& function_name = args->GetString(0);

		CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();

		if (context->Enter())
		{
			CefRefPtr<CefV8Value> js_function = context->GetGlobal()->GetValue(function_name);
			if (!js_function->IsFunction())
			{
				IPCLog(browser, "Renderer: Couldn't find JS function named '" + function_name + "' in current V8 context!"\
					" Aborting ExecuteJavascriptFunction routine!");
				context->Exit();
				return true;
			}

			// Transform parameters into CefV8Values
			CefV8ValueList params = {};
			for (int i = 1; i < args->GetSize(); i++)
				params.push_back(CefValueToCefV8Value(args->GetValue(i)));

			// Execute Javascript function
			CefRefPtr<CefV8Value> ret_val = js_function->ExecuteFunction(context->GetGlobal(), params);

			/* ### TODO ### Move to DOMNodeInteractionResponse.h */
			if (ret_val->IsObject())
			{
				std::vector<CefString> keys;
				ret_val->GetKeys(keys);

				const auto& iter = std::find(keys.begin(), keys.end(), CefString("command"));
				// Handle error case
				if (iter == keys.end())
				{
					IPCLog(browser, "Renderer: Invalid JS execution respond received! Aborting.");
					context->Exit();
					return true;
				}

				const std::string& command = ret_val->GetValue("command")->GetStringValue();
				// TODO: If command is known command!
				msg = CefProcessMessage::Create(command);
				const auto& args = msg->GetArgumentList();
				// TODO: This could be move to DOMNodeInteraction.h
				if (command == "EmulateEnterKey")
				{
					args->SetDouble(0, ret_val->GetValue("x")->GetDoubleValue());
					args->SetDouble(1, ret_val->GetValue("y")->GetDoubleValue());
					browser->SendProcessMessage(PID_BROWSER, msg);
					
				}

			}

			context->Exit();
			return true;
		}
		else
		{
			IPCLog(browser, "Renderer: Failed to enter V8 context in order to execute JS function '" + function_name + "'!");
		}
	}

	if (msgName == "SetSelectionIndex")
	{
		const auto& args = msg->GetArgumentList();

		CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();

		if (context->Enter())
		{
			const auto& window = context->GetGlobal();
			const auto& selectionFunc = window->GetValue("SetSelectionIndex");
			const auto& id = CefV8Value::CreateInt(args->GetInt(0));
			const auto& index = CefV8Value::CreateInt(args->GetInt(1));

			if (!selectionFunc->IsNull() && !selectionFunc->IsUndefined() && selectionFunc->IsFunction())
			{
				selectionFunc->ExecuteFunction(selectionFunc, { id, index });
			}
			
			context->Exit();
		}

	}


	if (msgName == "SendToLoggingMediator")
	{
		CefRefPtr<CefV8Value> log = CefV8Value::CreateString(msg->GetArgumentList()->GetString(0));

		CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();
		if (context->Enter())
		{
			CefRefPtr<CefV8Value> window = context->GetGlobal();
			CefRefPtr<CefV8Value> logMediator = window->GetValue("loggingMediator");
			if (logMediator->IsObject())
			{
				logMediator->GetValue("log")->ExecuteFunction(logMediator, { log });
			}

			context->Exit();
		}
	}

    // EXPERIMENTAL: Handle request of favicon image bytes
    if (msgName == "GetFavIconBytes")
    {
        CefRefPtr<CefFrame> frame = browser->GetMainFrame();
        CefRefPtr<CefV8Context> context = frame->GetV8Context();

		CefRefPtr<CefListValue> args = msg->GetArgumentList();
		int height = -2, width = -2;
		const std::string url = args->GetString(0);
		//height = args->GetInt(1);

        // Create process message, which is to be sent to Handler
        msg = CefProcessMessage::Create("ReceiveFavIconBytes");
        args = msg->GetArgumentList();

        if (context->Enter())
		{
            CefRefPtr<CefV8Value> globalObj = context->GetGlobal();

			if (globalObj->GetValue("favIconHeight")->IsDouble() && globalObj->GetValue("favIconWidth")->IsDouble())
			{
				height = globalObj->GetValue("favIconHeight")->GetDoubleValue();
				width = globalObj->GetValue("favIconWidth")->GetDoubleValue();

				// Fill msg args with help of this index variable
				int index = 0;
				// Write image resolution to IPC response
				args->SetInt(index++, width);
				args->SetInt(index++, height);

				if (width > 0 && height > 0)
				{
					IPCLogDebug(browser, "Reading bytes of favicon (w: " + std::to_string(width) + ", h: " + std::to_string(height) + ")");

					CefRefPtr<CefV8Value> byteArray = globalObj->GetValue("favIconData");

					// Fill byte array with JS
					browser->GetMainFrame()->ExecuteJavaScript(_js_favicon_copy_img_bytes_to_v8array, browser->GetMainFrame()->GetURL(), 0);

					// Read out each byte and write it to IPC msg
					//bool error_occured = false;
					for (int i = 0; i < byteArray->GetArrayLength(); i++)
					{
						//// Check if value IsInt will fail!
						//error_occured = error_occured || byteArray->GetValue(i)->IsInt();
						//if (error_occured)
						//{
						args->SetInt(index++, byteArray->GetValue(i)->GetIntValue());
						//}
					}
					
					//if(!error_occured)
					browser->SendProcessMessage(PID_BROWSER, msg);
					//else
					//{
					//	IPCLogDebug(browser, "An error occured while reading out favicon bytes!");
					//}
				}
				else
				{
					IPCLogDebug(browser, "Invalid favicon image resolution: w=" + std::to_string(width) + ", h=" + std::to_string(height));
				}
			}
			else
			{
				IPCLogDebug(browser, "Failed to load favicon height and/or width, expected double value, got something different. Aborting.");
			}
            context->Exit();
        }

    }

    if (msgName == "GetPageResolution")
    {
        CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();

        if (context->Enter())
        {
            CefRefPtr<CefV8Value> global = context->GetGlobal();

			if (global->GetValue("_pageWidth")->IsDouble() && global->GetValue("_pageHeight")->IsDouble())
			{
				double pageWidth = global->GetValue("_pageWidth")->GetDoubleValue();
				double pageHeight = global->GetValue("_pageHeight")->GetDoubleValue();

				msg = CefProcessMessage::Create("ReceivePageResolution");
				msg->GetArgumentList()->SetDouble(0, pageWidth);
				msg->GetArgumentList()->SetDouble(1, pageHeight);
				browser->SendProcessMessage(PID_BROWSER, msg);
			}
			else
			{
				IPCLogDebug(browser, "Failed to read page width and/or height, expected double value, got something different. Aborting.");
			}
            
            context->Exit();
        }
        return true;
    }

    // EXPERIMENTAL: Handle request of fixed elements' coordinates
    if (msgName == "FetchFixedElements")
    {
		

        CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();

        if (context->Enter())
        {
			int fixedId = msg->GetArgumentList()->GetInt(0);

			// Create response
			msg = CefProcessMessage::Create("ReceiveFixedElements");
			CefRefPtr<CefListValue> args = msg->GetArgumentList();

			CefRefPtr<CefV8Value> global = context->GetGlobal();
			

			CefRefPtr<CefV8Value> fixedObj;
			if (global->HasValue("domFixedElements") && fixedId < global->GetValue("domFixedElements")->GetArrayLength())
			{
				
				fixedObj = global->GetValue("domFixedElements")->GetValue(fixedId);

				// Slots in domFixedElements might contain undefined as value, if FixedElement was deleted
				if (fixedObj->IsUndefined() || fixedObj->IsNull())
				{
					//IPCLogDebug(browser, "Prevented access to already deleted fixed element at id=" + std::to_string(fixedId));
					context->Exit();
					return true;
				}
			}
			else
			{
				IPCLogDebug(browser, "List of fixed elements 'domFixedElements' does not seem to exist yet. Aborting fetching them.");
				context->Exit();
				return true;
			}
		
			int index = 0;
			args->SetInt(index++, fixedId);

			// Get V8 list of floats, representing all Rect coordinates of the given fixedObj
			CefRefPtr<CefV8Value> rectList = fixedObj->GetValue("getRects")->ExecuteFunction(fixedObj, {});

			if (rectList->IsUndefined() || rectList->IsNull() || rectList->GetArrayLength() == 0)
			{
				//IPCLogDebug(browser, "Fixed element's rects not available. Aborting...");
				// Abort
				context->Exit();
				return true;
			}

			for (int i = 0; i < rectList->GetArrayLength(); i++)
			{
				CefRefPtr<CefV8Value> rect = rectList->GetValue(i);
				if (rect->IsUndefined() || rect->IsNull())
					break;

				// Assuming each rect consist of exactly 4 double values
				for (int j = 0; j < rect->GetArrayLength(); j++)
				{
					// Access rect #i in rectList and j-th coordinate vale [t, l, b, r]
					args->SetDouble(index++, rect->GetValue(j)->GetDoubleValue());
				}

			}

			
			// Send response
			browser->SendProcessMessage(PID_BROWSER, msg);

        	context->Exit();
			return true;
        }
    }

	if (msgName == "FetchDOMTextLink" || msgName == "FetchDOMTextInput")
	{
		IPCLogDebug(browser, "Received deprecated "+msgName+" message!");
	}

	if (msgName.substr(0, 7) == "LoadDOM")
	{
		const std::string nodeType = msgName.substr(7, msgName.size());
		//IPCLogDebug(browser, "msg: '" + msgName + "'");
		//IPCLogDebug(browser, "nodeType: '" + nodeType + "'");
		const int id = msg->GetArgumentList()->GetInt(1);
		// msg->args[0] == nodeType as number, but not used here!

		// Fetch node type's attribute description
		std::vector<const std::vector<DOMAttribute>* > description;
		std::string js_obj_getter_name;

		DOM::GetJSRepresentation(nodeType, description, js_obj_getter_name); // TODO: nodeType string is currently a kind of a quick fix...

		if (description.size() == 0)
		{
			IPCLog(browser, "Renderer: Could not find fitting description for " + nodeType);
			return true;
		}

		CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();
		if (context->Enter())
		{
			CefRefPtr<CefV8Value> objGetter = context->GetGlobal()->GetValue(js_obj_getter_name);

			if (!objGetter->IsFunction())
			{
				IPCLog(browser, "Renderer: Could not find JS object getter function '" + js_obj_getter_name + "'.");
				return true;
			}

			// Call object getter and save returned object
			CefRefPtr<CefV8Value> domObj = objGetter->ExecuteFunction(context->GetGlobal(), { CefV8Value::CreateInt(id) });

			// Fill reply message with extracted attribute data
			CefRefPtr<CefProcessMessage> reply = CefProcessMessage::Create("ExtractedDOM" + nodeType);
			const auto& args = reply->GetArgumentList();
			int args_count = 0;

			args->SetInt(args_count++, id);

			for (const auto& desc : description)
			{
				for (const auto& attr : *desc)
				{
					CefRefPtr<CefListValue> listValue = V8ToCefListValue::ExtractAttributeData(attr, domObj, browser);

					args->SetList(args_count++, listValue);
				}
			}
			if (args->GetSize() <= 2)
			{
				LogDebug(browser, "Renderer: ERROR processing " + js_obj_getter_name + "(" + std::to_string(id) + ")!");
			}

			browser->SendProcessMessage(PID_BROWSER, reply);

			context->Exit();
		}

		return true;
	}

    // If no suitable handling was found, try message router
    return _msgRouter->OnProcessMessageReceived(browser, sourceProcess, msg);
}

void RenderProcessHandler::OnFocusedNodeChanged(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefDOMNode> node)
{
    // TODO, if needed
}

void RenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context)
{
	_msgRouter->OnContextCreated(browser, frame, context);

    if (frame->IsMain())
    {
		// TODO (Daniel): Really necessary?
		// Tell browser thread that context was created to discard all previous registered DOM nodes
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("OnContextCreated");
		browser->SendProcessMessage(PID_BROWSER, msg);


    // Create variables in Javascript which are to be read after page finished loading.
    // Variables here contain the amount of needed objects in order to allocate arrays, which are just big enough
        if (context->Enter())
        {
            // Retrieve the context's window object.
			CefRefPtr<CefV8Value> globalObj = context->GetGlobal();

			// TODO (Daniel): Are those still neccessary? There might be a better way!
            // Add attributes with their pre-set values to JS object |window|
            globalObj->SetValue("_pageWidth", CefV8Value::CreateDouble(-1), V8_PROPERTY_ATTRIBUTE_NONE);
            globalObj->SetValue("_pageHeight", CefV8Value::CreateDouble(-1), V8_PROPERTY_ATTRIBUTE_NONE);
            globalObj->SetValue("sizeTextLinks", CefV8Value::CreateInt(0), V8_PROPERTY_ATTRIBUTE_NONE);
            globalObj->SetValue("sizeTextInputs", CefV8Value::CreateInt(0), V8_PROPERTY_ATTRIBUTE_NONE);

            // Create JS variables for width and height of favicon image
            globalObj->SetValue("favIconHeight", CefV8Value::CreateInt(-1), V8_PROPERTY_ATTRIBUTE_NONE);
            globalObj->SetValue("favIconWidth", CefV8Value::CreateInt(-1), V8_PROPERTY_ATTRIBUTE_NONE);

			// Create an image object, which will later contain favicon image 
            frame->ExecuteJavaScript(_js_favicon_create_img, frame->GetURL(), 0);

			// Inject Javascript code which extends the current page's context by our methods
			// and automatically creates a MutationObserver instance
			for (const auto& dom_code : _js_dom_code)
			{
				frame->ExecuteJavaScript(dom_code.first, dom_code.second, 0);
			}
			const auto& add_attribute = context->GetGlobal()->GetValue("AddDOMAttribute");
			if (add_attribute->IsFunction())
			{
				bool valid = true;
				int attrId = 0;
				while (valid)
				{
					const std::string& attrStr = DOMAttrToString((DOMAttribute) attrId);
					valid = (attrStr.size() >= 3);	// There won't exist any attribute name with less than 4 characters
					if (valid)
						add_attribute->ExecuteFunction(
							context->GetGlobal(),
							{ CefV8Value::CreateString(attrStr), CefV8Value::CreateInt(attrId) }
					);
					attrId++;
				}
			}
			else
			{
				IPCLog(browser, "Renderer: ERROR: Could not find JS function 'AddDOMAttribute'!");
			}
			frame->ExecuteJavaScript("MutationObserverInit();", "", 0);


			//IPCLog(browser, "LOADING FIXED ELEMENT JS FILE OVER AND OVER AGAIN");
			//_js_dom_fixed_elements = GetJSCode(DOM_FIXED_ELEMENTS);
			//frame->ExecuteJavaScript(_js_dom_fixed_elements, "", 0);



            context->Exit();
        }
        /*
        *	GetFavIconBytes
        * END *******************************************************************************/

    }
    //else IPCLogDebug(browser, "Not able to enter context! (main frame?="+std::to_string(frame->IsMain())+")");
}

void RenderProcessHandler::OnContextReleased(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context)
{
	_msgRouter->OnContextReleased(browser, frame, context);

    if (frame->IsMain())
    {
		IPCLogDebug(browser, "### Context released for main frame. ###");

        // Release all created V8 values, when context is released
        CefRefPtr<CefV8Value> globalObj = context->GetGlobal();

        globalObj->DeleteValue("_pageWidth");
        globalObj->DeleteValue("_pageHeight");

        globalObj->DeleteValue("sizeTextLinks");
        globalObj->DeleteValue("sizeTextInputs");

        globalObj->DeleteValue("favIconHeight");
        globalObj->DeleteValue("favIconWidth");

		// DEBUG
		// frame->ExecuteJavaScript("MutationObserverShutdown()", "", 0);
    }
}

void RenderProcessHandler::IPCLog(CefRefPtr<CefBrowser> browser, std::string text, bool debugLog)
{
    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("IPCLog");
    msg->GetArgumentList()->SetBool(0, debugLog);
    msg->GetArgumentList()->SetString(1, text);
    browser->SendProcessMessage(PID_BROWSER, msg);

    // Just in case (time offset in log file due to slow IPC msg, for example): Use CEF's logging as well
    if (debugLog)
    {
        DLOG(INFO) << text;
    }
    else
    {
        LOG(INFO) << text;
    }
}
