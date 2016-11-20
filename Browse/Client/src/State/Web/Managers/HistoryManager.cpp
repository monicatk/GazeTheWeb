//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "HistoryManager.h"
#include "src/Global.h"
#include "src/Utils/Logger.h"
#include "submodules/eyeGUI/externals/TinyXML2/tinyxml2.h"
#include <iterator>

HistoryManager::HistoryManager(std::string userDirectory)
{
	// Fill members
	_fullpathHistory = userDirectory + HISTORY_FILE;

	// Load existing history
	if (!LoadHistory()) { LogInfo("HistoryManager: No history file found or parsing error"); }
}

HistoryManager::~HistoryManager()
{
	// Nothing to do
}

void HistoryManager::AddPage(std::string URL, std::string title)
{
	// Create new struct
	Page page;
	page.URL = URL;
	page.title = title;

	// Add to vector with pages
	_pages.push_back(page);

	// Try to open XML file
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError result = doc.LoadFile(_fullpathHistory.c_str());

	// Try to create XML file on failure
	tinyxml2::XMLNode* pRoot = NULL;
	if (result != tinyxml2::XMLError::XML_SUCCESS)
	{
		// Create and insert root
		tinyxml2::XMLNode* pRoot = doc.NewElement("history");
		doc.InsertFirstChild(pRoot);

		// Try to save file
		tinyxml2::XMLError result = doc.SaveFile(_fullpathHistory.c_str());

		// Return at failure
		if (result != tinyxml2::XMLError::XML_SUCCESS)
		{
			LogInfo("HistoryManager: Failed to create history file");
			return;
		}
	}

	// Append to XML file
	// TODO
}

std::vector<HistoryManager::Page> HistoryManager::GetHistory() const
{
	return _pages;
}

bool HistoryManager::LoadHistory()
{
	// Clean history
	_pages.clear();

	// Open document
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError result = doc.LoadFile(_fullpathHistory.c_str());

	// Check result of opening 
	if (result != tinyxml2::XMLError::XML_SUCCESS) { return false; }

	// Fetch root
	tinyxml2::XMLNode* pRoot = doc.FirstChild();
	if (pRoot == NULL) { return false; }

	// Get first child
	tinyxml2::XMLElement* pElement = pRoot->FirstChildElement("page");
	if (pElement == NULL) { return true; } // nothing found but somehow successful

	// Collect pages
	do
	{
		// Preparation
		Page page;
		bool pageError = true;

		// Query URL string from attribute
		const char * pURL = NULL;
		pURL = pElement->Attribute("url");
		if (pageError |= (pURL != NULL))
		{
			page.URL = pURL;
		}

		// Query title string from attribute
		const char * pTitle = NULL;
		pTitle = pElement->Attribute("title");
		if (pageError |= (pTitle != NULL))
		{
			page.title = pTitle;
		}

		// Add page only when no error occured
		if (!pageError)
		{
			_pages.push_back(page);
		}
	} while ((pElement = pElement->NextSiblingElement("bookmark")) != NULL);

	// When you came to here no real errors occured
	return true;
}