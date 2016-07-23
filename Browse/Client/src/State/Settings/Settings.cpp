//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Settings.h"
#include "src/Master.h"
#include "src/Global.h"

Settings::Settings(Master* pMaster) : State(pMaster)
{
	// Intial setup
	_pMaster->SetShowDescriptions(_globalSetup._showDescriptions);
	_pMaster->SetGazeVisualization(_globalSetup._showGazeVisualization);

	// Create layouts
	_pSettingsLayout = _pMaster->AddLayout("layouts/Settings.xeyegui", EYEGUI_SETTINGS_LAYER, false);
	_pGeneralLayout = _pMaster->AddLayout("layouts/SettingsGeneral.xeyegui", EYEGUI_SETTINGS_LAYER, false);
	_pInfoLayout = _pMaster->AddLayout("layouts/SettingsInfo.xeyegui", EYEGUI_SETTINGS_LAYER, false);

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

void Settings::SettingsButtonListener::down(eyegui::Layout* pLayout, std::string id)
{
	if (pLayout == _pSettings->_pSettingsLayout)
	{
		// ### Settings layout ###
		if (id == "close")
		{
			_pSettings->_goToWeb = true;
		}
		else if (id == "general")
		{
			eyegui::setVisibilityOfLayout(_pSettings->_pGeneralLayout, true, true, true);
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
			_pSettings->_globalSetup._showDescriptions = !_pSettings->_globalSetup._showDescriptions;
			_pSettings->_pMaster->SetShowDescriptions(_pSettings->_globalSetup._showDescriptions);
		}
		else if (id == "toggle_gaze_visualization")
		{
			_pSettings->_globalSetup._showGazeVisualization = !_pSettings->_globalSetup._showGazeVisualization;
			_pSettings->_pMaster->SetGazeVisualization(_pSettings->_globalSetup._showGazeVisualization);
		}
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