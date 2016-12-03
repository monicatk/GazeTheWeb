//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "RenderProcessHandler.h"
#include "include/wrapper/cef_message_router.h"
#include "include/base/cef_logging.h"
#include "include/wrapper/cef_helpers.h"
#include <sstream>

RenderProcessHandler::RenderProcessHandler()
{
	CefMessageRouterConfig config;
	config.js_query_function = "cefQuery";
	config.js_cancel_function = "cefQueryCancel";
	_msgRouter = CefMessageRouterRendererSide::Create(config);
}

bool RenderProcessHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId sourceProcess,
    CefRefPtr<CefProcessMessage> msg)
{
    CEF_REQUIRE_RENDERER_THREAD();

    const std::string& msgName = msg->GetName().ToString();
    //IPCLogDebug(browser, "Received '" + msgName + "' IPC msg in RenderProcessHandler");

    // Handle request of DOM node data
    if (msgName == "GetDOMElements")
    {
		IPCLogDebug(browser, "Renderer: Reached old code 'GetDOMElements' msg");
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
					bool error_occured = false;
					for (int i = 0; i < byteArray->GetArrayLength(); i++)
					{
						error_occured = error_occured || byteArray->GetValue(i)->IsInt();
						if (error_occured)
						{
							args->SetInt(index++, byteArray->GetValue(i)->GetIntValue());
						}
					}
					
					if(!error_occured)
						browser->SendProcessMessage(PID_BROWSER, msg);
				}
				else
				{
					IPCLogDebug(browser, "Invalid favicon image resolution: w=" + std::to_string(width) + ", h=" + std::to_string(height));
				}
			}
			else
			{
				IPCLogDebug(browser, "Renderer: Failed to load favicon height and/or width, expected double value, got something different. Aborting.");
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
				IPCLogDebug(browser, "Renderer: Failed to read page width and/or height, expected double value, got something different. Aborting.");
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
			if (global->HasValue("domFixedElements"))
			{
				fixedObj = global->GetValue("domFixedElements")->GetValue(fixedId);
			}
			else
			{
				IPCLogDebug(browser, "Renderer: List of fixed elements does not seem to exist yet. Aborting fetching them.");
				return true;
			}
			
			
			//CefRefPtr<CefV8Value> rectsArray = fixedObj->GetValue("rects");
			
			int index = 0;
			args->SetInt(index++, fixedId);

			// Get V8 list of floats, representing all Rect coordinates of the given fixedObj
			CefRefPtr<CefV8Value> rectList = fixedObj->GetValue("getRects")->ExecuteFunction(fixedObj, {});

			for (int i = 0; i < rectList->GetArrayLength(); i++)
			{
				// Assuming each rect consist of exactly 4 double values
				for (int j = 0; j < 4; j++)
				{
					// Access rect #i in rectList and j-th coordinate vale [t, l, b, r]
					args->SetDouble(index++, rectList->GetValue(i)->GetValue(j)->GetDoubleValue());
				}

			}

			// DEBUG
			if (rectList->GetArrayLength() == 0)
			{
				IPCLogDebug(browser, "No Rect coordinates available for fixedID="+std::to_string(fixedId));
			}
      
			// Send response
			browser->SendProcessMessage(PID_BROWSER, msg);

        	context->Exit();
        }
    }

	if (msgName == "FetchDOMTextLink")
	{
		IPCLog(browser, "Renderer: Received Deprecated FetchDOMTextLink message!");
	}

	if (msgName == "FetchDOMTextInput")
	{
		IPCLog(browser, "Renderer: Received Deprecated FetchDOMTextInput message!");
	}

	if (msgName == "LoadDOMNodeData")
	{
		//IPCLogDebug(browser, "Received 'LoadDOMNodeData' msg...");

		// Generic fetching of node data of 'nodeID' for a specific node type 'type'
		CefRefPtr<CefListValue> args = msg->GetArgumentList();
		int type = args->GetInt(0);
		int nodeID = args->GetInt(1);

		CefRefPtr<CefV8Context> context = browser->GetMainFrame()->GetV8Context();
		// Get DOMObject which wraps the DOM node itself
		CefRefPtr<CefV8Value> domObj = FetchDOMObject(context, type, nodeID);

		// Unwrap DOMObject and specify its node type
		switch (type)
		{
			case 0: { msg = UnwrapDOMTextInput(context, domObj, nodeID); break; };
			case 1: { msg = UnwrapDOMLink(context, domObj, nodeID); break; };
			default: { msg = NULL; break; };
		}

		if (msg && msg->GetArgumentList()->GetSize() > 0)
		{
			// Send DOMNode data to browser process
			browser->SendProcessMessage(PID_BROWSER, msg);
		}
		else
		{
			IPCLogDebug(browser, "ERROR: Could not create DOMNode id=" + std::to_string(nodeID) + ", type=" + std::to_string(type) + "!");
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
		// Tell browser thread that context was created to discard all previous registered DOM nodes
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("OnContextCreated");
		browser->SendProcessMessage(PID_BROWSER, msg);


    // Create variables in Javascript which are to be read after page finished loading.
    // Variables here contain the amount of needed objects in order to allocate arrays, which are just big enough
        if (context->Enter())
        {
			// DEBUG
			IPCLogDebug(browser, "### Context created for main frame. ###");

            // Retrieve the context's window object.
			CefRefPtr<CefV8Value> globalObj = context->GetGlobal();


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

			//IPCLogDebug(browser, "DEBUG: Renderer reloads JS code for MutationObserver on EVERY context creation atm!");
			//_js_mutation_observer_test = GetJSCode(MUTATION_OBSERVER_TEST);
			//_js_dom_mutationobserver = GetJSCode(DOM_MUTATIONOBSERVER);

			frame->ExecuteJavaScript(_js_dom_mutationobserver, "", 0);
			frame->ExecuteJavaScript(_js_mutation_observer_test, "", 0);
			frame->ExecuteJavaScript(_js_dom_fixed_elements, "", 0);
			frame->ExecuteJavaScript("MutationObserverInit();", "", 0);

			// TESTING
		/*	CefRefPtr<CefV8Value> myObj = globalObj->GetValue("myObject");
			CefV8ValueList args;
			if (myObj->GetValue("getVal")->IsFunction())
			{
				IPCLogDebug(browser, "IsFunction!");
				CefRefPtr<CefV8Value> returned = myObj->GetValue("getVal")->ExecuteFunctionWithContext(context, myObj, args);
				if (returned)
				{
					if (returned->IsInt())
						IPCLogDebug(browser, "Return value is an int");
					if (returned->IsUndefined())
						IPCLogDebug(browser, "Return value is undefined");
					if (returned->IsNull())
						IPCLogDebug(browser, "Return value is NULL");
				}

			}
			else
			{
				IPCLogDebug(browser, "Is NOT a function!");
			}*/

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
		frame->ExecuteJavaScript("MutationObserverShutdown()", "", 0);
    }
}

void RenderProcessHandler::IPCLogDebug(CefRefPtr<CefBrowser> browser, std::string text)
{
    IPCLog(browser, text, true);
}

/**
 *	Returns the JS DOMObject which wraps the DOMNode with the given type and ID
 */
CefRefPtr<CefV8Value> RenderProcessHandler::FetchDOMObject(CefRefPtr<CefV8Context> context, int nodeType, int nodeID)
{
	CefRefPtr<CefV8Value> result = CefV8Value::CreateNull();

	if (context->Enter())
	{
		CefRefPtr<CefV8Value> jsWindow = context->GetGlobal();

		// Only try to execute function, if function exists
		if (jsWindow->GetValue("GetDOMObject")->IsFunction())
		{
			// 
			result = jsWindow->GetValue("GetDOMObject")->ExecuteFunction(
				NULL, 
				{ CefV8Value::CreateInt(nodeType), CefV8Value::CreateInt(nodeID) }
			);
		}
		else
		{
			IPCLogDebug(context->GetBrowser(), "ERROR: Could not find function 'GetDOMObject(nodeType, nodeID)' in Javascript context!");
		}

		context->Exit();

	}
	else // Couldn't enter context
	{
		IPCLogDebug(context->GetBrowser(), "ERROR: Could not load DOMTextInput with id="+std::to_string(nodeID)+ ", because context could not be entered!");
	}

	return result;
}

CefRefPtr<CefProcessMessage> RenderProcessHandler::UnwrapDOMTextInput(
	CefRefPtr<CefV8Context> context, 
	CefRefPtr<CefV8Value> domObj, 
	int nodeID)
{
	CefRefPtr<CefProcessMessage> result = CefProcessMessage::Create("CreateDOMTextInput");
	CefRefPtr<CefListValue> args = result->GetArgumentList();

	if (domObj && !domObj->IsNull())
	{
		if (context->Enter())
		{
			int index = 0;
			args->SetInt(index++, 0);	// nodeType = 0 for DOMTextInput
			args->SetInt(index++, nodeID);
			args->SetBool(index++, domObj->GetValue("visible")->GetBoolValue());

			// Get V8 list of floats, representing all Rect coordinates of the given domObj
			CefRefPtr<CefV8Value> rectsData = domObj->GetValue("getRects")->ExecuteFunction(domObj, {});
			for (int i = 0; i < rectsData->GetArrayLength(); i++)
			{
				for (int j = 0; j < 4; j++)
				{
					args->SetDouble(index++, rectsData->GetValue(i)->GetValue(j)->GetDoubleValue());
				}
			}

			context->Exit();
		}
	}
	else
	{
		IPCLogDebug(context->GetBrowser(), "ERROR: Given DOMObject is NULL! Could not read out DOMTextInput");
	}

	return result;
}

CefRefPtr<CefProcessMessage> RenderProcessHandler::UnwrapDOMLink(
	CefRefPtr<CefV8Context> context,
	CefRefPtr<CefV8Value> domObj,
	int nodeID)
{
	CefRefPtr<CefProcessMessage> result = CefProcessMessage::Create("CreateDOMLink");
	CefRefPtr<CefListValue> args = result->GetArgumentList();

	if (domObj && !domObj->IsNull())
	{
		if (context->Enter())
		{
			int index = 0;
			args->SetInt(index++, 1);	// nodeType = 1 for DOMLinks
			args->SetInt(index++, nodeID);
			args->SetBool(index++, domObj->GetValue("visible")->GetBoolValue());

			// Get V8 list of floats, representing all Rect coordinates of the given domObj
			CefRefPtr<CefV8Value> rectsData = domObj->GetValue("getRects")->ExecuteFunction(domObj, {});
			for (int i = 0; i < rectsData->GetArrayLength(); i++)
			{
				for (int j = 0; j < 4; j++)
				{
					args->SetDouble(index++, rectsData->GetValue(i)->GetValue(j)->GetDoubleValue());
				}
			}

			context->Exit();
		}
	}
	else
	{
		IPCLogDebug(context->GetBrowser(), "ERROR: Given DOMObject is NULL! Could not read out DOMLink");
	}

	return result;
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
