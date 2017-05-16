//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Settings.h"
#include "src/Master.h"
#include "src/Global.h"
#include "src/Utils/Logger.h"
#include "submodules/eyeGUI/externals/TinyXML2/tinyxml2.h"

// Include singleton for mailing to JavaScript
#include "src/Singletons/JSMailer.h"

Settings::Settings(Master* pMaster) : State(pMaster)
{
	// Initialize members
	_fullpathSettings = pMaster->GetUserDirectory() + SETTINGS_FILE;

	// Load settings and apply them
	bool saveSettings = false;
	if (!LoadSettings())
	{
		LogInfo("Settings: No settings file found or parsing error");
		saveSettings = true;
	}
	ApplySettings(saveSettings); // sets settings

	// Create layouts
	_pSettingsLayout = _pMaster->AddLayout("layouts/Settings.xeyegui", EYEGUI_SETTINGS_LAYER, false);
	_pGeneralLayout = _pMaster->AddLayout("layouts/SettingsGeneral.xeyegui", EYEGUI_SETTINGS_LAYER, false);
	_pInfoLayout = _pMaster->AddLayout("layouts/SettingsInfo.xeyegui", EYEGUI_SETTINGS_LAYER, false);

    // Set state of switches before registering listerners to avoid unnecessary saving
    if(_globalSetup.showDescriptions) { eyegui::buttonDown(_pGeneralLayout, "toggle_descriptions", true); }
    if(_globalSetup.showGazeVisualization) { eyegui::buttonDown(_pGeneralLayout, "toggle_gaze_visualization", true); }

	// Button listener
	_spSettingsButtonListener = std::shared_ptr<SettingsButtonListener>(new SettingsButtonListener(this));
	eyegui::registerButtonListener(_pSettingsLayout, "close", _spSettingsButtonListener);
	eyegui::registerButtonListener(_pSettingsLayout, "general", _spSettingsButtonListener);
	eyegui::registerButtonListener(_pSettingsLayout, "info", _spSettingsButtonListener);
	eyegui::registerButtonListener(_pSettingsLayout, "exit", _spSettingsButtonListener);
	eyegui::registerButtonListener(_pGeneralLayout, "toggle_descriptions", _spSettingsButtonListener);
	eyegui::registerButtonListener(_pGeneralLayout, "toggle_gaze_visualization", _spSettingsButtonListener);
	eyegui::registerButtonListener(_pGeneralLayout, "back", _spSettingsButtonListener);
	eyegui::registerButtonListener(_pInfoLayout, "back", _spSettingsButtonListener);

	// Deactivate buttons which are not used, yet
	eyegui::setElementActivity(_pSettingsLayout, "privacy", false, false);
	eyegui::setElementActivity(_pSettingsLayout, "input", false, false);
}

Settings::~Settings()
{
	// Delete layouts?
}

StateType Settings::Update(float tpf, Input& rInput)
{
    if (_goToWeb)
	{
		return StateType::WEB;
	}
	else
	{
		return StateType::SETTINGS;
	}
}

void Settings::Draw() const
{

}

void Settings::Activate()
{
	// Super
	State::Activate();

	// Make layout visible
	eyegui::setVisibilityOfLayout(_pSettingsLayout, true, true, true);

	// Reset stuff
	_goToWeb = false;
}

void Settings::Deactivate()
{
	// Super
	State::Deactivate();

	// Make all layouts invisible
	eyegui::setVisibilityOfLayout(_pSettingsLayout, false, false, true);
}

// Save settings to hard disk. Returns whether successful
bool Settings::SaveSettings() const
{
	// Create document
	tinyxml2::XMLDocument doc;

	// Create and insert root
	tinyxml2::XMLNode* pRoot = doc.NewElement("settings");
	doc.InsertFirstChild(pRoot);

	// Create environment for global setup
	tinyxml2::XMLNode* pGlobal = doc.NewElement("global");
	pRoot->InsertFirstChild(pGlobal);

	// Descriptions
	tinyxml2::XMLElement* pDescriptions = doc.NewElement("descriptions");
	pDescriptions->SetAttribute("visible", _globalSetup.showDescriptions ? "true" : "false");
	pGlobal->InsertFirstChild(pDescriptions);

	// Gaze visualization
	tinyxml2::XMLElement* pGazeVisualization = doc.NewElement("gazevisualization");
	pGazeVisualization->SetAttribute("visible", _globalSetup.showGazeVisualization ? "true" : "false");
	pGlobal->InsertAfterChild(pDescriptions, pGazeVisualization);

	// Keyboard layout
	tinyxml2::XMLElement* pKeyboardLayout = doc.NewElement("keyboardlayout");
	std::string layout;
	switch (_globalSetup.keyboardLayout)
	{
	case eyegui::KeyboardLayout::US_ENGLISH:
		layout = "US_ENGLISH";
		break;
	case eyegui::KeyboardLayout::GERMANY_GERMAN:
		layout = "GERMANY_GERMAN";
		break;
	case eyegui::KeyboardLayout::ISRAEL_HEBREW:
		layout = "ISRAEL_HEBREW";
		break;
	case eyegui::KeyboardLayout::GREECE_GREEK:
		layout = "GREECE_GREEK";
		break;
	}
	pKeyboardLayout->SetAttribute("layout", layout.c_str());
	pGlobal->InsertAfterChild(pGazeVisualization, pKeyboardLayout);

	// Create environment for web setup
	tinyxml2::XMLNode* pWeb = doc.NewElement("web");
	pRoot->InsertAfterChild(pGlobal, pWeb);

	// Homepage
	tinyxml2::XMLElement* pHomepage = doc.NewElement("homepage");
	pHomepage->SetAttribute("url", _webSetup.homepage.c_str());
	pWeb->InsertFirstChild(pHomepage);
	
	// Try to save file
	tinyxml2::XMLError result = doc.SaveFile(_fullpathSettings.c_str());

	// Return whether successful
	return (result == tinyxml2::XMLError::XML_SUCCESS);
}

void Settings::ApplySettings(bool save)
{
	// Global
	_pMaster->SetShowDescriptions(_globalSetup.showDescriptions);
	_pMaster->SetGazeVisualization(_globalSetup.showGazeVisualization);
	
	// TODO: if keyboard layout can be set from options, one cannot call "SetKeyboardLayout"
	// in master from here, as it would call this again. Think of a better way in case you want
	// to change it both from application and settings.

	// Save it
	if (save)
	{
		if (!SaveSettings()) { LogInfo("Settings: Failed to save settings"); }
	}
}

// Load settings from hard disk. Returns whether successful
bool Settings::LoadSettings()
{
	// Open document
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError result = doc.LoadFile(_fullpathSettings.c_str());

	// Check result of opening 
	if (result != tinyxml2::XMLError::XML_SUCCESS) { return false; }

	// Fetch root
	tinyxml2::XMLNode* pRoot = doc.FirstChild();
	if (pRoot == NULL) { return false; }

	// Fetch global
	tinyxml2::XMLNode* pGlobal = pRoot->FirstChild();
	if (pGlobal == NULL) { return false; }

	// Descriptions
	tinyxml2::XMLElement* pDescriptions = pGlobal->FirstChildElement("descriptions");
	if (pDescriptions != NULL)
	{
		_globalSetup.showDescriptions = pDescriptions->BoolAttribute("visible");
	}

	// Gaze visualization
	tinyxml2::XMLElement* pGazeVisualization = pGlobal->FirstChildElement("gazevisualization");
	if (pGazeVisualization != NULL)
	{
		_globalSetup.showGazeVisualization = pGazeVisualization->BoolAttribute("visible");
	}

	// Keyboard layout
	tinyxml2::XMLElement* pKeyboardLayout = pGlobal->FirstChildElement("keyboardlayout");
	if (pKeyboardLayout != NULL)
	{
		std::string attribute = pKeyboardLayout->Attribute("layout");
		if (attribute == "GERMANY_GERMAN")
		{
			_globalSetup.keyboardLayout = eyegui::KeyboardLayout::GERMANY_GERMAN;
		}
		else if (attribute == "ISRAEL_HEBREW")
		{
			_globalSetup.keyboardLayout = eyegui::KeyboardLayout::ISRAEL_HEBREW;
		}
		else if (attribute == "GREECE_GREEK")
		{
			_globalSetup.keyboardLayout = eyegui::KeyboardLayout::GREECE_GREEK;
		}
		else // "US_ENGLISH" as fallback if string is different
		{
			_globalSetup.keyboardLayout = eyegui::KeyboardLayout::US_ENGLISH;
		}
	}

	// Fetch web setup
	tinyxml2::XMLNode* pWeb = pGlobal->NextSibling();
	if (pWeb == NULL) { return false; }

	// Homepage
	tinyxml2::XMLElement* pHomepage = pWeb->FirstChildElement("homepage");
	if (pHomepage != NULL)
	{
		const char * pURL = NULL;
		pURL = pHomepage->Attribute("url");
		if (pURL != NULL)
		{
			_webSetup.homepage = std::string(pURL);
		}
	}

	// When you came to here no real errors occured
	return true;
}

void Settings::SettingsButtonListener::down(eyegui::Layout* pLayout, std::string id)
{
	if (pLayout == _pSettings->_pSettingsLayout)
	{
		// ### Settings layout ###
		if (id == "close")
		{
			_pSettings->_goToWeb = true;
			JSMailer::instance().Send("close");
		}
		else if (id == "general")
		{
			eyegui::setVisibilityOfLayout(_pSettings->_pGeneralLayout, true, true, true);
			JSMailer::instance().Send("general");
		}
		else if (id == "info")
		{
			eyegui::setVisibilityOfLayout(_pSettings->_pInfoLayout, true, true, true);
		}
		else if (id == "exit")
		{
			_pSettings->_pMaster->Exit();
		}
	}
	else if (pLayout == _pSettings->_pGeneralLayout)
	{
		// ### General layout ###
		if (id == "back")
		{
			eyegui::setVisibilityOfLayout(_pSettings->_pGeneralLayout, false, false, true);
		}
		else if (id == "toggle_descriptions")
		{
            _pSettings->_globalSetup.showDescriptions = true;
		}
		else if (id == "toggle_gaze_visualization")
		{
            _pSettings->_globalSetup.showGazeVisualization = true;
			JSMailer::instance().Send("gaze_on");
		}

		// Apply and save
		_pSettings->ApplySettings(true);
	}
	else if (pLayout == _pSettings->_pInfoLayout)
	{
		// ### Information layout ###
		if (id == "back")
		{
			eyegui::setVisibilityOfLayout(_pSettings->_pInfoLayout, false, false, true);
		}
	}
}

void Settings::SettingsButtonListener::up(eyegui::Layout* pLayout, std::string id)
{
    if (pLayout == _pSettings->_pGeneralLayout)
    {
        // ### General layout ###
        if (id == "toggle_descriptions")
        {
            _pSettings->_globalSetup.showDescriptions = false;
        }
        else if (id == "toggle_gaze_visualization")
        {
            _pSettings->_globalSetup.showGazeVisualization = false;
			JSMailer::instance().Send("gaze_off");
        }

        // Apply and save
        _pSettings->ApplySettings(true);
    }
}
