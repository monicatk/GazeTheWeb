//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "History.h"
#include "src/Global.h"
#include "src/Master.h"
#include "src/Utils/Helper.h"


History::History(Master* pMaster, HistoryManager const * pHistoryManager)
{
	// Fill members
	_pMaster = pMaster;
	_pHistoryManager = pHistoryManager;

	// Create layouts
	_pLayout = _pMaster->AddLayout("layouts/History.xeyegui", EYEGUI_WEB_HISTORY_LAYER, false);

	// Create listener
	_spHistoryButtonListener = std::shared_ptr<HistoryButtonListener>(new HistoryButtonListener(this));
	eyegui::registerButtonListener(_pLayout, "close", _spHistoryButtonListener);
}

History::~History()
{
	_pMaster->RemoveLayout(_pLayout);
}

bool History::Update()
{
	return _finished;
}

void History::Activate(int tabId)
{
	if (!_active)
	{
		// Make layout visible
		eyegui::setVisibilityOfLayout(_pLayout, true, true, true);

		// Reset collected URL
		_collectedURL = u"";

		// Finished, not yet
		_finished = false;

		// Remember id of tab which url shall be changed
		_currentTabId = tabId;

		// Remember activation
		_active = true;

		// TODO: fetch history?
	}
}

void History::Deactivate()
{
	if (_active)
	{
		// Make layouts invisible
		eyegui::setVisibilityOfLayout(_pLayout, false, false, true);

		// Remember it
		_active = false;
	}
}

std::string History::GetURL() const
{
	std::string url8;
	eyegui_helper::convertUTF16ToUTF8(_collectedURL, url8);
	return url8;
}

void History::HistoryButtonListener::down(eyegui::Layout* pLayout, std::string id)
{
	// Standard layout
	if (id == "close")
	{
		// Reset URL to empty to indicate abortion
		_pHistory->_collectedURL = u"";
		_pHistory->_finished = true;
	}
}