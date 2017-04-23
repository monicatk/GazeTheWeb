//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#include "MessageRouter.h"
#include "src/CEF/Mediator.h"
#include "src/Utils/Logger.h"
#include "src/CEF/Data/DOMNode.h"
#include "src/CEF/Data/DOMExtraction.h"
#include <cstdlib>
#include <algorithm>

MessageRouter::MessageRouter(Mediator* pMediator)
{
	// Store pointer to mediator
	_pMediator = pMediator;

	// Create configuration for CEF message router
	CefMessageRouterConfig config;
	config.js_query_function = "cefQuery";
	config.js_cancel_function = "cefQueryCancel";
	_router = CefMessageRouterBrowserSide::Create(config);

	// Add the default handler for messages to the delegated router
	CefMessageRouterBrowserSide::Handler* defaultHandler = new DefaultMsgHandler(_pMediator);
	_router->AddHandler(defaultHandler, true);
}

bool DefaultMsgHandler::OnQuery(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int64 query_id,
	const CefString& request,
	bool persistent,
	CefRefPtr<Callback> callback)
{
	const std::string requestString = request.ToString();

	// ###############
	// ### Favicon ###
	// ###############

	if (requestString == "faviconBytesReady")
	{
		// Logging
		LogDebug("MessageRouter: Received 'faviconBytesReady' callback from Javascript");

		// Tell renderer to read out favicon image's bytes
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("GetFavIconBytes");
		browser->SendProcessMessage(PID_RENDERER, msg);

		// Success!
		callback->Success("success");
		return true;
	}

	// ######################
	// ### Text Selection ###
	// ######################

	// Text selection callback, asynchronously called when CefMediator::EndTextSelection finishes
	if (requestString.compare(0, 8, "#select#") == 0)
	{
		// Split result
		auto split = SplitBySeparator(requestString, '#');

		// Check whether result is empty
		if (split.size() > 1)
		{
			// Extract selection
			const std::string selectionStr = split.at(1);

			// Logging
			LogDebug("MsgRouter: Selected Text: '", selectionStr, "'");

			// Set clipboard in mediator (TODO: what happens when two parallel string extractions are ongoing? some weird overwriting)
			_pMediator->SetClipboardText(selectionStr);
		}
	
		// Success!
		callback->Success("success");
		return true;
	}

	// #####################
	// ### Fixed Element ###
	// #####################

	// Fixed element callbacks
	if (requestString.compare(0, 9, "#fixElem#") == 0)
	{
		std::vector<std::string> data = SplitBySeparator(requestString, '#');
		const std::string& op = data[1];

		if (data.size() > 2)
		{
			const std::string& id = data[2];

			if (op.compare("rem") == 0) // removing fixed element
			{
				// Notify Tab via Mediator that a fixed element was removed
				_pMediator->RemoveFixedElement(browser, atoi(id.c_str()));

				// Success!
				callback->Success("success");
				return true;
			}
			if (op.compare("add") == 0) // adding fixed element
			{
				// Tell RenderProcessHandler to read out bounding rectangle coordinates belonging to the given ID
				CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("FetchFixedElements");
				msg->GetArgumentList()->SetInt(0, atoi(id.c_str()));
				browser->SendProcessMessage(PID_RENDERER, msg);

				// Success!
				callback->Success("success");
				return true;
			}
		}
		else
		{
			// Failure!
			LogDebug("MsgRouter: Expected more arguments for fixed elements. Aborting.");
			callback->Failure(-1, "not enough arguments");
			return true;
		}
	}

	// ################
	// ### DOM Node ###
	// ################
	// DOM#{add | rem | upd}#nodeType#nodeID{#attribute#data}#
	// Definitions:
	//	add--> send process message to Renderer to fetch V8 data
	//	rem--> request removal of given node
	//	upd--> update data of an specific node attribute
	//	nodeType: int
	//		0 : TextInput
	//		1 : TextLink
	//	nodeID : int
	//	attribute : int
	//		0 : Rect
	//		1 : _fixed
	//	data : depends on attribute
	// Example: DOM#upd#7#1337#0#0.9;0.7;0.5;0.7#

	if (requestString.compare(0, 4, "DOM#") == 0)
	{
		std::vector<std::string> data = SplitBySeparator(requestString, '#');

		if (data.size() > 3)
		{
			const std::string& op = data[1];
			const int& type = std::stoi(data[2]);
			const int& id = std::stoi(data[3]);
		
			//LogDebug(requestString, " id: ", id);
			//if (type == 3) // DEBUG: OverflowElement
			//	LogDebug("MsgRouter: Processing " + requestString);

			// ADDING DOMNODE
			if (op.compare("add") == 0) // adding of DOM node
			{
				// Create blank node object in corresponding Tab object
				switch (type)
				{
					case(0): {_pMediator->AddDOMTextInput(browser, id); break; }
					case(1): {_pMediator->AddDOMLink(browser, id); break; }
					case(2): {_pMediator->AddDOMSelectField(browser, id); break; }
					case(3): {_pMediator->AddDOMOverflowElement(browser, id); break; }
					default: {
						LogError("MsgRouter: Adding DOMNode - Unknown type of DOMNode! type=", type);
					}
				}

				// TODO: This could be done in DOMExtraction
				std::string ipcName = "";
				switch (type) {
				case(0) : ipcName = "TextInput"; break;
				case(1) : ipcName = "Link"; break;
				case(2) : ipcName = "SelectField"; break;
				case(3) : ipcName = "OverflowElement"; break;
				default: LogError(browser, "MsgRouter: - ERROR: Unknown numeric DOM node type value: ", type);
				}

				// Instruct Renderer Process to initialize empty DOM Nodes with data
				CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("LoadDOM" + ipcName + "Data");
				msg->GetArgumentList()->SetInt(0, type);
				msg->GetArgumentList()->SetInt(1, id);
				browser->SendProcessMessage(PID_RENDERER, msg);
			}

			// REMOVE DOMNODE
			if (op.compare("rem") == 0)
			{
				switch (type)
				{
					case(0): {_pMediator->RemoveDOMTextInput(browser, id); break; }
					case(1): {_pMediator->RemoveDOMLink(browser, id); break; }
					case(2): {_pMediator->RemoveDOMSelectField(browser, id); break; }
					case(3) : {_pMediator->RemoveDOMOverflowElement(browser, id); break; }
					default: {
						LogError("MsgRouter: Removing DOMNode - Unknown type of DOMNode! type=", type);
					}
				}
			}

			// UPDATE DOMNODE
			if (op.compare("upd") == 0)
			{
				std::weak_ptr<DOMNode> target;
				switch (type)
				{
					case(0): {target = _pMediator->GetDOMTextInput(browser, id); break; }
					case(1): {target = _pMediator->GetDOMLink(browser, id); break; }
					case(2): {target = _pMediator->GetDOMSelectField(browser, id); break; }
					case(3) : {target = _pMediator->GetDOMOverflowElement(browser, id); break; }
					default: {
						LogError("MsgRouter: Updating DOMNode - Unknown type of DOMNode! type=", type);
					}
				}

				if (data.size() > 5)
				{
					// See DOMAttribute.h enum DOMAttribute for numeric interpretation
					const DOMAttribute& attr = (DOMAttribute) std::stoi(data[4]);
					const std::string& attrData = data[5];



					// Perform node update
					if (auto node = target.lock())
					{
						node->Update(
							(DOMAttribute) attr,
							StringToCefListValue::ExtractAttributeData((DOMAttribute) attr, attrData)
						);
					}
				}
				else
				{
					LogDebug("MsgRouter: Expected more data in DOMObject update:\n", requestString, "\nAborting update.");
				}
			}

		}
		
		// Success! (TODO: there may be failures which can be passed to JavaScript)
		callback->Success("success");
		return true;
	}

	// If request is unknown & couldn't be handled, assume it to be a ConsolePrint call in JS and log it
	LogDebug("Javascript: ", requestString);

	return false;
}

bool CallbackMsgHandler::OnQuery(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int64 query_id,
	const CefString& request,
	bool persistent,
	CefRefPtr<Callback> callback)
{
	const std::string requestString = request.ToString();
	
	// Compare prefix of request with given prefix
	if (requestString.substr(0, _prefix.size()).compare(_prefix) == 0)
	{
		// Remove prefix
		std::string message = requestString.substr(_prefix.size());

		// Call callback
		_callbackFunction(message);
		callback->Success("success"); // tell JavaScript about success
		return true;
	}

	return false;
}