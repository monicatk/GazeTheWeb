//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "BrowserMsgRouter.h"
#include "src/CEF/Extension/CefMediator.h"
#include "src/Utils/Logger.h"
#include <cstdlib>
#include "src/State/Web/Tab/DOMNode.h"

BrowserMsgRouter::BrowserMsgRouter(CefMediator* pMediator)
{
	_pMediator = pMediator;

	// Create configuration for browser side message router
	CefMessageRouterConfig config;
	config.js_query_function = "cefQuery";
	config.js_cancel_function = "cefQueryCancel";

	// Create and add the core message router
	_router = CefMessageRouterBrowserSide::Create(config);

	// Create and add msgRouter for msg handling
	CefMessageRouterBrowserSide::Handler* myHandler = new MsgHandler(this);
	_router->AddHandler(myHandler, true);
}

MsgHandler::MsgHandler(BrowserMsgRouter* pMsgRouter)
{
	_pMsgRouter = pMsgRouter;
}

bool MsgHandler::OnQuery(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int64 query_id,
	const CefString& request,
	bool persistent,
	CefRefPtr<Callback> callback)
{
	const std::string requestName = request.ToString();

	if (requestName == "faviconBytesReady")
	{
		callback->Success("GetFavIconBytes");
		LogDebug("BrowserMsgRouter: Received 'faviconBytesReady 'callback from Javascript");

		// Tell renderer to read out favicon image's bytes
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("GetFavIconBytes");
		browser->SendProcessMessage(PID_RENDERER, msg);

		return true;
	}

	// Fixed element callbacks
	if (requestName.compare(0, 9, "#fixElem#") == 0)
	{
		if (requestName.compare(9, 4, "rem#") == 0)
		{
			std::string id = requestName.substr(13, 2);
			LogDebug("BrowserMsgRouter: Fixed element #", id, " was removed.");

			// Notify Tab via CefMediator, that a fixed element was removed
			_pMsgRouter->GetMediator()->RemoveFixedElement(browser, atoi(id.c_str()));

			return true;
		}
		if (requestName.compare(9, 4, "add#") == 0)
		{
			std::string id = requestName.substr(13, 2);
			LogDebug("BrowserMsgRouter: Fixed element #", id, " was added.");

			// Tell Renderer to read out bounding rectangle coordinates belonging to the given ID
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("FetchFixedElements");
			msg->GetArgumentList()->SetInt(0, atoi(id.c_str()));
			browser->SendProcessMessage(PID_RENDERER, msg);

			return true;
		}
		
	}

	

	/* NOTES
		Encoding of request strings for DOM node operations:

		DOM#{add | rem | upd}#nodeType#nodeID{#attribute#data}#


		Definitions:
		nodeType	: int
			0 : TextInput
			1 : TextLink
		nodeID		: int
		add --> send process message to Renderer to fetch V8 data
		rem --> request removal of given node
		upd --> update data of an specific node attribute
		attribute	: int
			0	: Rect
			1	: _fixed
		data	: depends on attribute
	*/

	// Identify general DOM request
	// example: DOM#upd#7#1337#5#0.9;0.7;0.5;0.7#
	if (requestName.compare(0, 4, "DOM#") == 0)
	{
		const char typeChar = requestName.at(8);
		DOMNodeType type;
		switch (typeChar)
		{
		case('0') : {type = DOMNodeType::TextInput; break;  };
		case('1') : {type = DOMNodeType::TextLink; break;  };
		}

		// Extract information of variable length from rest of string
		int id = -1, attr = -1;
		std::string dataStr;

		const std::string str = requestName.substr(10, requestName.length() - 1);
		std::vector<char> buffer;
		int amount_separators = 0;
		for (int i = 0; i < (int)str.length(); i++)
		{
			const char read = str.at(i);
			if (read != '#')
			{
				buffer.push_back(read);
			}
			else
			{
				// Write char buffer to string
				const std::string bufferStr(buffer.begin(), buffer.end());

				// DEBUG
				//LogDebug(i, ": bufferStr= ", bufferStr);

				// Identify what information is to be extracted
				switch (amount_separators)
				{
				case(0) : { 
					id = std::stoi(bufferStr); break;
				};
				case(1) : { 
					attr = std::stoi(bufferStr);
					// Extract data string, interpret it later
					dataStr = str.substr(i + 1, str.length() - 2);
					break; 
				};
				}
				amount_separators++;
				buffer.clear();
			}
		}
		// DEBUG
		//LogDebug("MsgHandler: Received 'DOM' request for nodeID=", id, " with nodeType=", type);
		//LogDebug("MsgHandler: str= ", str);
		//LogDebug("MsgHandler: dataStr= ", dataStr);


		// Node was added
		if (requestName.compare(4, 3, "add") == 0)
		{
			//LogDebug("MsgHandler: Javascript says, that DOM node with type=", type, " & id=", id, " was found.");

			// Create blank DOM node of given node type with nodeID
			_pMsgRouter->GetMediator()->AddDOMNode(
				browser,
				std::make_shared<DOMNode>(
					type,
					browser->GetMainFrame()->GetIdentifier(),
					id,
					Rect()
					)
				);

			// Send IPC msg to Renderer to fetch more data to fill in
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("LoadDOMNodeData");
			msg->GetArgumentList()->SetInt(0, type);
			msg->GetArgumentList()->SetInt(1, id);
			browser->SendProcessMessage(PID_RENDERER, msg);
		}

		// Node was removed
		if (requestName.compare(4, 3, "rem") == 0)
		{
			_pMsgRouter->GetMediator()->RemoveDOMNode(browser, type, id);
		}

		// Node was updated
		if (requestName.compare(4, 3, "upd") == 0)
		{
			void* data = nullptr;
			switch (attr)
			{
				// Rect was updated, extract 4 double values OLD
				case(0) : {
					std::vector<float> rectData;
					std::vector<char> buffer;
					for (int i = 0; i < (int)dataStr.length(); i++)
					{
						const char read = dataStr.at(i);
						if (read != ';' && read != '#')
						{
							buffer.push_back(read);
						}
						else if (buffer.size() > 0)
						{
							const std::string bufferStr(buffer.begin(), buffer.end());
							rectData.push_back(std::stod(bufferStr));

							// DEBUG
							//LogDebug("MsgRouter: Read ", bufferStr, " converted it to ", std::stod(bufferStr));

							buffer.clear();
						}
					}
					// TODO: shared_ptr anstatt void* !

					std::vector<Rect> rects;
					LogDebug("Str encoding of node id=", id, ", type=", type);
					for (int i = 0; i < rectData.size(); i++)
						LogDebug(i, ": ", rectData[i]);
					for (int i = 0; i + 3 < rectData.size(); i+= 4)
					{
						//Rect rect = Rect(std::vector<float>(rectData.data() + i, rectData.data() + i + 3));
						Rect rect = Rect(rectData[i], rectData[i + 1], rectData[i + 2], rectData[i + 3]);
						rects.push_back(rect);
						LogDebug(i, ": " + rect.toString());
					}
					//Rect rect = Rect(rectData);
					// Assign extracted data to |data|
					LogDebug("size rects: ", rects.size());
					data = (void*) &rects;

					// TODO: Einheitliche Kodierung einführen!
					attr = 3;

					break;
				};

				// Nodes fixation status changed
				case(1) : {
					//LogDebug("request= ", requestName);
					//LogDebug("dataStr= ", dataStr);
				
					bool boolVal = dataStr.at(0) != '0';
					// Assign extracted data to |data|
					data = (bool*)&(boolVal);

					//LogDebug("data= ", *(bool*)data);
					break;
				};
			}

			// Pass decoded update information to Tab via CefMediator
			_pMsgRouter->GetMediator()->UpdateDOMNode(browser, type, id, attr, data);
		} // End of node update

		return true;
	}

	// Print message to console and withdraw callback
	LogDebug("Javascript: ", requestName);
	callback->Failure(0, "");

	return false;
}