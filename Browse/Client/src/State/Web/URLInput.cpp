//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "URLInput.h"
#include "src/Global.h"
#include "src/Master.h"

URLInput::URLInput(Master* pMaster)
{
    // Fill members
    _pMaster = pMaster;

    // Create layout
    _pLayout = _pMaster->AddLayout("layouts/URLInput.xeyegui", EYEGUI_WEB_URLINPUT_LAYER, false);

    // Create listener
    _spURLKeyboardListener = std::shared_ptr<URLKeyboardListener>(new URLKeyboardListener(this));
    eyegui::registerKeyboardListener(_pLayout, "keyboard", _spURLKeyboardListener);
    _spURLButtonListener = std::shared_ptr<URLButtonListener>(new URLButtonListener(this));
    eyegui::registerButtonListener(_pLayout, "close", _spURLButtonListener);
    eyegui::registerButtonListener(_pLayout, "delete", _spURLButtonListener);
    eyegui::registerButtonListener(_pLayout, "complete", _spURLButtonListener);
}

URLInput::~URLInput()
{
    _pMaster->RemoveLayout(_pLayout);
}

bool URLInput::Update()
{
    return _finished;
}

void URLInput::Activate(int tabId)
{
    if (!_active)
    {
        // Make layout visible
        eyegui::setVisibilityOfLayout(_pLayout, true, true, true);

        // Reset collected URL
        _collectedURL = u"";

        // Set initial content of text block TODO: localization
        eyegui::setContentOfTextBlock(_pLayout, "text_block", "URL will appear here");

        // Finished, not yet
        _finished = false;

        // Remember id of tab which url shall be changed
        _currentTabId = tabId;

        // Remember it
        _active = true;
    }
}

void URLInput::Deactivate()
{
    if (_active)
    {
        // Make layout invisible
        eyegui::setVisibilityOfLayout(_pLayout, false, true, true);

        // Remember it
        _active = false;
    }
}

std::string URLInput::GetURL() const
{
	std::string url8;
	eyegui_helper::convertUTF16ToUTF8(_collectedURL, url8);
	return url8;
}

void URLInput::URLKeyboardListener::keyPressed(eyegui::Layout* pLayout, std::string id, std::u16string value)
{
    _pURLInput->_collectedURL += value;
    eyegui::setContentOfTextBlock(_pURLInput->_pLayout, "text_block", _pURLInput->_collectedURL + u"|");
}

void URLInput::URLButtonListener::down(eyegui::Layout* pLayout, std::string id)
{
    if (id == "close")
    {
        // Reset URL input to empty to indicate abortion
        _pURLInput->_collectedURL = u"";
        _pURLInput->_finished = true;
    }
    else if (id == "delete")
    {
        if (_pURLInput->_collectedURL.size() > 0)
        {
            // Delete last letter
            _pURLInput->_collectedURL.pop_back();

            // Tell preview about it
            eyegui::setContentOfTextBlock(_pURLInput->_pLayout, "text_block", _pURLInput->_collectedURL);

            // TODO: Displaying does not work for empty string aka last letter deleted
        }
    }
    else if (id == "complete")
    {
        _pURLInput->_finished = true;
    }
}
