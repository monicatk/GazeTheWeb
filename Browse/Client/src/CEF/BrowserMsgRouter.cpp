//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "BrowserMsgRouter.h"
#include "src/CEF/Extension/CefMediator.h"
#include "src/Utils/Logger.h"
#include "src/CEF/Data/DOMNode.h"
#include <cstdlib>
#include <algorithm>

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

std::vector<std::string> MsgHandler::SplitBySeparator(std::string str, char separator)
{
	std::vector<std::string> output;
	std::vector<char> buffer;

	for (int i = 0; i < str.length(); i++)
	{
		const char read = str.at(i);
		if (read == separator)
		{
			if (buffer.size() > 0)
			{
				const std::string bufferStr(buffer.begin(), buffer.end());
				output.push_back(bufferStr);
				buffer.clear();
			}
			// Insert empty strings between two separators, but not directly after first separator!
			else if (i > 0 && buffer.size() == 0)
			{
				output.push_back("");
			}
		}
		else
		{
			buffer.push_back(read);
		}
	}
	if (buffer.size() > 0)
	{
		const std::string bufferStr(buffer.begin(), buffer.end());
		output.push_back(bufferStr);
		buffer.clear();
	}

	return output;
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
		LogDebug("BrowserMsgRouter: Received 'faviconBytesReady' callback from Javascript");

		// Tell renderer to read out favicon image's bytes
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("GetFavIconBytes");
		browser->SendProcessMessage(PID_RENDERER, msg);

		return true;
	}

	// Text selection callback, asynchronously called when CefMediator::EndTextSelection finishes
	if (requestName.compare(0, 8, "#select#") == 0)
	{
		// Split result
		auto split = SplitBySeparator(requestName, '#');

		// Check whether result is empty
		if (split.size() > 1)
		{
			// Extract selection
			const std::string selectionStr = split.at(1);

			LogDebug("MsgRouter: Selected Text: '", selectionStr, "'");

			// Set clipboard in mediator (TODO: what happens when two parallel string extractions are ongoing? some weird overwriting)
			_pMsgRouter->GetMediator()->SetClipboardText(selectionStr);
		}
	
		return true;
	}

	// Fixed element callbacks
	if (requestName.compare(0, 9, "#fixElem#") == 0)
	{
		std::vector<std::string> data = SplitBySeparator(requestName, '#');
		const std::string& op = data[1];

		if (data.size() > 2)
		{
			const std::string& id = data[2];

			if (op.compare("rem") == 0)
			{

				// Notify Tab via CefMediator, that a fixed element was removed
				_pMsgRouter->GetMediator()->RemoveFixedElement(browser, atoi(id.c_str()));

				return true;
			}
			if (op.compare("add") == 0)
			{
				// Tell Renderer to read out bounding rectangle coordinates belonging to the given ID
				CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("FetchFixedElements");
				msg->GetArgumentList()->SetInt(0, atoi(id.c_str()));
				browser->SendProcessMessage(PID_RENDERER, msg);
				return true;
			}
		}
		else
		{
			LogDebug("MsgRouter: Expected more arguments for fixed elements. Aborting.");
			return true;
		}
		
	}

	if (requestName.compare(0, 9, "#ovrflow#") == 0)
	{
		if (requestName.compare(9, 4, "add#") == 0)
		{
			std::string dataStr = requestName.substr(13, requestName.length());
			std::vector<std::string> data = SplitBySeparator(dataStr, '#');

			//Extract OverflowElement ID from dataStr
			int id = std::stoi(data[0]);

			 //Extract Rect data from encoded String
			std::vector<float> rectData;
			std::vector<std::string> rectStrData = SplitBySeparator(data[1], ';');
			std::for_each(
				rectStrData.begin(),
				rectStrData.end(), 
				[&rectData](std::string str) {rectData.push_back(std::stod(str)); }
			);
			Rect rect = Rect(rectData);

			// Extract maximum possible scrolling in x and y direction
			std::vector<std::string> scrollMaxsStr = SplitBySeparator(data[2], ';');
			if (scrollMaxsStr[0].compare("undefined") == 0 || scrollMaxsStr[1].compare("undefined") == 0)
			{
				LogDebug("MsgRouter: Error in OverflowElement creation: Missing max scrolling value(s)! Setting them to 0.");
			}
			int scrollLeftMax = (scrollMaxsStr[0].compare("undefined") == 0) ? 0 : std::stoi(scrollMaxsStr[0]);
			int scrollTopMax = (scrollMaxsStr[1].compare("undefined") == 0) ? 0 : std::stoi(scrollMaxsStr[1]);


			OverflowElement overflowElem = OverflowElement(id, rect, scrollLeftMax, scrollTopMax);

			_pMsgRouter->GetMediator()->AddOverflowElement(browser, std::make_shared<OverflowElement>(overflowElem));

			return true;
		}
		if (requestName.compare(9, 4, "rem#") == 0)
		{
			std::vector<std::string> dataStr = SplitBySeparator(requestName.substr(13), '#');
			_pMsgRouter->GetMediator()->RemoveOverflowElement(browser, std::stoi(dataStr[0]));

			return true;
		}
		if (requestName.compare(9, 4, "upd#") == 0)
		{
			std::vector<std::string> dataStr = SplitBySeparator(requestName.substr(13), '#');

			// Assuming elementId, attribute name & data are enough information (at this time)
			if (dataStr.size() == 3)
			{
				// Overflow Element ID
				int id = std::stoi(dataStr[0]);

				// Overflow Elements attribute as string, which gets updated
				std::string attr = dataStr[1];

				// dataStr[2] contains data for given attributes value changes

				// Get access to given Overflow Element in Tab
				std::weak_ptr<OverflowElement> wpElem = _pMsgRouter->GetMediator()->GetOverflowElement(browser, id);
				if (const auto& elem = wpElem.lock())
				{
					// Update Overflow Element's rect data
					if (attr == "rect")
					{
						std::vector<std::string> rectData = SplitBySeparator(dataStr[2], ';');
						std::vector<float> rect;
						// Convert Rect coordinates from string to float
						std::for_each(
							rectData.begin(),
							rectData.end(),
							[&rect](std::string str) {rect.push_back(std::stof(str)); }
						);

						// Use float coordinates to update Rect #0
						elem->UpdateRect(0, std::make_shared<Rect>(rect));
					}

					// Update OverflowElement's fixation status
					if (attr == "fixed")
					{
						elem->UpdateFixation(std::stoi(dataStr[2]));
					}

				}
			}
			else
			{
				LogError("MsgRouter: An error occured in decoding the update String of an OverflowElement!");

			}

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
	// example: DOM#upd#7#1337#0#0.9;0.7;0.5;0.7#
	if (requestName.compare(0, 4, "DOM#") == 0)
	{
		std::vector<std::string> data = SplitBySeparator(requestName, '#');

		if (data.size() > 3)
		{
			const std::string& op = data[1];
			const int& numeric_type = std::stoi(data[2]);
			const int& id = std::stoi(data[3]);
			
			// Identify type as string with DOMNodeType
			DOMNodeType type;
			switch (numeric_type)
			{
			case(0) : {type = DOMNodeType::TextInput; break;  };
			case(1) : {type = DOMNodeType::TextLink; break;  };
			}

			// Node was added
			if (op.compare("add") == 0)
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
			if (op.compare("rem") == 0)
			{
				_pMsgRouter->GetMediator()->RemoveDOMNode(browser, type, id);
			}

			// Node was updated
			if (op.compare("upd") == 0)
			{
				if (data.size() > 5)
				{
					const int& attr = std::stoi(data[4]);
					const std::string& attrData = data[5];
					
					switch (attr)
					{
						// List of Rects were updated
						case(0) : {
							std::vector<std::string> rectDataStr = SplitBySeparator(attrData, ';');
							std::vector<float> rectData;
							// Extract each float value from string, earlier separated by ';', and convert string to numerical value
							std::for_each(
								rectDataStr.begin(),
								rectDataStr.end(), 
								[&rectData](std::string value) {rectData.push_back(std::stod(value)); }
							);

							std::vector<Rect> rects;
							// Read out each 4 float values und create 1 Rect with them
							for (int i = 0; i + 3 < rectData.size(); i += 4)
							{
								Rect rect = Rect(rectData[i], rectData[i + 1], rectData[i + 2], rectData[i + 3]);
								rects.push_back(rect);
							}

							// Get weak_ptr to target node and get shared_ptr targetNode out of weak_ptr
							if (auto targetNode = _pMsgRouter->GetMediator()->GetDOMNode(browser, type, id).lock())
							{
								targetNode->SetRects(std::make_shared<std::vector<Rect>>(rects));
							}
							break;
						};

						// Node's fixation status changed
						case(1) : {
							bool boolVal = attrData.at(0) != '0';

							// Get weak_ptr to target node and get shared_ptr targetNode out of weak_ptr
							if (auto targetNode = _pMsgRouter->GetMediator()->GetDOMNode(browser, type, id).lock())
							{
								targetNode->SetFixed(boolVal);
							}

							break;
						};

						// Node's visibility has changed
						case (2) : {
							bool boolVal = attrData.at(0) != '0';

							// Get weak_ptr to target node and get shared_ptr targetNode out of weak_ptr
							if (auto targetNode = _pMsgRouter->GetMediator()->GetDOMNode(browser, type, id).lock())
							{
								targetNode->SetVisibility(boolVal);
							}

							break;
						}
						// Node's text(Content) changed
						case (3) : {
							if (auto targetNode = _pMsgRouter->GetMediator()->GetDOMNode(browser, type, id).lock())
							{
								targetNode->SetText(attrData);
								LogDebug("MsgRouter: Set node's text attribute to: '", attrData, "'");
							}
							break;
						}
						default: {
							LogDebug("MsgHandler: Received Javascript 'update' request of DOMNode attribute=", attr, ", which is not yet defined.");
						}
					}
				}
				else
				{
					LogDebug("MsgRouter: Expected more data in DOMObject update:\n", requestName, "\nAborting update.");
					return true;
				}
			}
		}

		return true;
	}

	// Print message to console and withdraw callback
	LogDebug("Javascript: ", requestName);
	callback->Failure(0, "");

	return false;
}
