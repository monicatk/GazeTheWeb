//============================================================================
// Distributed under the MIT License.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "SimpleRenderProcessHandler.h"
#include "include/base/cef_logging.h"

#include <sstream>

bool SimpleRenderProcessHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId sourceProcess,
    CefRefPtr<CefProcessMessage> msg)
{
    const std::string& msgName = msg->GetName().ToString();

	if (msgName == "GetJSCoords")
	{
		// TODO: Type checking if it is truely a string!
		CefRefPtr<CefV8Context> context = browser->GetFocusedFrame()->GetV8Context();
		//if (context->Enter())
		//{
			std::stringstream ss;
			ss << "var inputNodes = document.getElementsByTagName('input');"
				"var textInput = [];"
				// extract all text input nodes, which do not have identical coordinates!
				"for (i = 0; i < inputNodes.length; i++)"
				"   if(inputNodes[i].type == 'text' || inputNodes[i].type == 'search' || inputNodes[i].type == 'email' || inputNodes[i].type == 'password')"
				"   {"
				"      var rect1 = inputNodes[i].getBoundingClientRect();"
				// skip singularities..
				"      if(rect1.height == 0 || rect1.height == 0)"
				"         continue;"
				"      if(textInput.length > 0)"
				"      {"
				// test nodes for identical coordinates
				"         var rect2 = textInput[textInput.length - 1].getBoundingClientRect();" // (?) performance of this whole comparing rects?
				"         if(rect1.top != rect2.top || rect1.bottom != rect2.bottom || rect1.left != rect2.left || rect1.right != rect2.right)"
				"            textInput.push(inputNodes[i]);"
				"      }"
				"      else"
				"         textInput.push(inputNodes[i]);"
				"   }"
				/// "var bodyRect = document.body.getBoundingClientRect();" // Position des <body>
				// DEBUG: get some information with alert(out) at the end
				///"var out = 'Found '+inputNodes.length+' input nodes and '+textInput.length+' text input node(s) on this page! ';"
				// fill C++readable JS variables with text input coordinates
				"for(i = 0; i < textInput.length; i++){"
				// get position of each text input node
				"   var elemRect = textInput[i].getBoundingClientRect();"
				"   window.txtInput_top[i] = elemRect.top;"
				"   window.txtInput_bottom[i] = elemRect.bottom;"
				"   window.txtInput_left[i] = elemRect.left;"
				"   window.txtInput_right[i] = elemRect.right;"
				"}"
				// debug output if not enough space is given for amount of text input fields
				///"if (textInput.length > window.txtInput_top.length)"
				///"window.debug_string += '// There '+textInput.length-window.txtInput_top.length+' text input fields than available coordinate storage!';"
				// DEBUG: get some feedback
				///"alert(out);"
				//"alert(window.debug_string);";
				;
			browser->GetFocusedFrame()->ExecuteJavaScript(ss.str(), browser->GetFocusedFrame()->GetURL(), 0);

			// browser viewport origin: top-left

			// TODO: send coordinates & field's width & height to browser process
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("JSCoords");
			if (context->Enter())
			{
				CefRefPtr<CefV8Value> object = context->GetGlobal();
				for (unsigned int i = 0; i < FETCHED_JS_TEXT_INPUTS; i++)
				{
					msg->GetArgumentList()->SetDouble(i * 4, object->GetValue("txtInput_top")->GetValue(i)->GetDoubleValue());
					msg->GetArgumentList()->SetDouble(i * 4 + 1, object->GetValue("txtInput_bottom")->GetValue(i)->GetDoubleValue());
					msg->GetArgumentList()->SetDouble(i * 4 + 2, object->GetValue("txtInput_left")->GetValue(i)->GetDoubleValue());
					msg->GetArgumentList()->SetDouble(i * 4 + 3, object->GetValue("txtInput_right")->GetValue(i)->GetDoubleValue());
				}
				browser->SendProcessMessage(PID_BROWSER, msg);
				context->Exit();
			}


				////DEBUG
				//msg = CefProcessMessage::Create("DEBUG");
				//msg->GetArgumentList()->SetString(0, std::to_string(object->GetValue("txtInput_top[0]")->GetDoubleValue()));
				//browser->SendProcessMessage(PID_BROWSER, msg);
			return true;
		
	}
       

   // OLD
  //  if (msgName == "KeyboardInput")
  //  {
		//const int index = msg->GetArgumentList()->GetInt(0);
  //      const std::string txt = msg->GetArgumentList()->GetString(1);

  //      std::stringstream ss;
		//ss << "textInput[" + std::to_string(index) + "].value = '" + txt + "';" 
		//	<< "alert('worked!');";
		//		
  //      browser->GetFocusedFrame()->ExecuteJavaScript(ss.str(), browser->GetFocusedFrame()->GetURL(), 0);

		//// give some feedback
		//CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("InputSuccess");
		//browser->SendProcessMessage(PID_BROWSER, msg);

  //      return true;
  //  }

    if (msgName == "InputSubmit")
    {
        CefRefPtr<CefV8Context> context = browser->GetFocusedFrame()->GetV8Context();
        if (context->Enter())
        {
            const int index = msg->GetArgumentList()->GetInt(0);
            const std::string txt = msg->GetArgumentList()->GetString(1);

            std::stringstream ss;
            ss << "textInput[" + std::to_string(index) + "].submit()';"
                << "alert('Submit!');"
                ;

            browser->GetFocusedFrame()->ExecuteJavaScript(ss.str(), browser->GetFocusedFrame()->GetURL(), 0);

            context->Exit();
        }
        else LOG(ERROR) << "Can not enter context!";
        return true;
    }

    return false;
}

void SimpleRenderProcessHandler::OnFocusedNodeChanged(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefDOMNode> node)
{
    // TODO, if needed
}

void SimpleRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context)
{
    if (context->Enter())
    {
        // Retrieve the context's window object.
        CefRefPtr<CefV8Value> object = context->GetGlobal();
        // Create a new V8 string value. See the "Basic JS Types" section below.
        CefRefPtr<CefV8Value> str = CefV8Value::CreateString("JS debug_string: ");
        // Add the string to the window object as "window.myval"
        object->SetValue("debug_string", str, V8_PROPERTY_ATTRIBUTE_NONE);

        // Create arrays for all top, bottom, left, right values in JS
        std::vector<CefRefPtr<CefV8Value>> coords; // = {top, bottom, left, right}
        std::vector<std::string> coordNames = { "txtInput_top" , "txtInput_bottom" ,"txtInput_left", "txtInput_right" };
        for (int i = 0; i < 4; i++)
        {
            coords.push_back(CefV8Value::CreateArray(FETCHED_JS_TEXT_INPUTS));
            for (unsigned int j = 0; j < FETCHED_JS_TEXT_INPUTS; j++)
            {
                coords[i]->SetValue(j, CefV8Value::CreateDouble(-1));
            }
            object->SetValue(coordNames[i], coords[i], V8_PROPERTY_ATTRIBUTE_NONE);
        }
        //CefRefPtr<CefV8Value> txtTop = CefV8Value::CreateArray(FETCHED_JS_TEXT_INPUTS),
        //	txtLeft = CefV8Value::CreateArray(FETCHED_JS_TEXT_INPUTS),
        //	txtRight = CefV8Value::CreateArray(FETCHED_JS_TEXT_INPUTS),
        //	txtBottom = CefV8Value::CreateArray(FETCHED_JS_TEXT_INPUTS);
        //object->SetValue(, txtTop, V8_PROPERTY_ATTRIBUTE_NONE);
        //object->SetValue(, txtBottom, V8_PROPERTY_ATTRIBUTE_NONE);
        //object->SetValue(, txtLeft, V8_PROPERTY_ATTRIBUTE_NONE);
        //object->SetValue(, txtRight, V8_PROPERTY_ATTRIBUTE_NONE);

        context->Exit();
    } else DLOG(ERROR) << "Not able to enter context!";
}
