//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Web state which manages the tabs. Interactions are displayed on own layout.

#ifndef WEB_H_
#define WEB_H_

#include "src/State/State.h"
#include "src/State/Web/Tab/Tab.h"
#include "src/State/Web/Managers/BookmarkManager.h"
#include "src/State/Web/Managers/HistoryManager.h"
#include "src/State/Web/Screens/URLInput.h"
#include "src/State/Web/Screens/History.h"
#include <map>
#include <vector>
#include <memory>
#include <stack>

// Forward declaration
class CefMediator;

class Web : public State, public WebTabInterface
{
public:

    // Constructor
    Web(Master* pMaster, CefMediator* pCefMediator);

    // Destructor
    virtual ~Web();

    // Add Tab and return id of it
    int AddTab(std::string URL, bool show = true);

    // Add Tab after another
    int AddTabAfter(Tab* other, std::string URL, bool show = true);

    // Remove Tab
    void RemoveTab(int id);

    // Switch to tab. Returns whether successful
    bool SwitchToTab(int id);

    // Switch to tab by index. Returns whether successful
    bool SwitchToTabByIndex(int index);

    // Switch to next tab. Returns whether successful
    bool SwitchToNextTab();

    // Switch to previous tab. Returns whether successful
    bool SwitchToPreviousTab();

    // Opens URL in tab. Returns whether successful
    bool OpenURLInTab(int id, std::string URL);

    // #############
    // ### STATE ###
    // #############

    // Update. Returns which state should be active in next time step
    virtual StateType Update(float tpf, Input& rInput);

    // Draw
    virtual void Draw() const;

    // Activate
    virtual void Activate();

    // Deactivate
    virtual void Deactivate();

	// #########################
	// ### WEB TAB INTERFACE ###
	// #########################

	// Add tab after that tab
    virtual void PushAddTabAfterJob(Tab* pCaller, std::string URL);

	// Add page to history job
	virtual void PushAddPageToHistoryJob(Tab* pCaller, HistoryManager::Page page);

    // Get own id in web. Returns -1 if not found
    virtual int GetIdOfTab(Tab const * pCaller) const;

private:

    // Jobs given by Tab over WebTabInterface
    class TabJob
    {
    public:

        // Constructor
        TabJob(Tab* pCaller);

        // Execute
        virtual void Execute(Web* pCallee) = 0;

    protected:

        // Members
        Tab* _pCaller;
    };

    class AddTabAfterJob : public TabJob
    {
    public:

        // Constructor
		AddTabAfterJob(Tab* pCaller, std::string URL, bool show) : TabJob(pCaller)
		{
			_URL = URL;
			_show = show;
		}

        // Execute
        virtual void Execute(Web* pCallee);

    protected:

        // Members
        std::string _URL;
        bool _show;
    };

	class AddPageToHistoryJob : public TabJob
	{
	public:

		// Constructor
		AddPageToHistoryJob(Tab* pCaller, HistoryManager::Page page) : TabJob(pCaller)
		{
			_page = page;
		}

		// Execute
		virtual void Execute(Web* pCallee);

	protected:

		// Members
		HistoryManager::Page _page;
	};

    // Give listener full access
    friend class WebButtonListener;

    // Listener for GUI
    class WebButtonListener: public eyegui::ButtonListener
    {
    public:

        WebButtonListener(Web* pWeb) { _pWeb = pWeb; }
        virtual void hit(eyegui::Layout* pLayout, std::string id) {}
        virtual void down(eyegui::Layout* pLayout, std::string id);
        virtual void up(eyegui::Layout* pLayout, std::string id) {}

    private:

        Web* _pWeb;
    };

    // Instance of listener
    std::shared_ptr<WebButtonListener> _spWebButtonListener;

    // Get index of Tab in order vector. Returns -1 if not found
    int GetIndexOfTabInOrderVector(int id) const;

    // Show tab overview
    void ShowTabOverview(bool show);

    // Update tab overview
    void UpdateTabOverview();

    // Calculate page cound for tab overview
    int CalculatePageCountOfTabOverview() const;

	// Update icon of tab overview
	void UpdateTabOverviewIcon();

    // Maps id to Tab
    std::map<int, std::unique_ptr<Tab> > _tabs;

    // Order of tabs, saving ids in vector
    std::vector<int> _tabIdOrder;

    // Current tab is indicated with index of vector (-1 means, that no tab is currently displayed)
    int _currentTabId = -1;

    // Layouts
    eyegui::Layout* _pWebLayout;
    eyegui::Layout* _pTabOverviewLayout;

    // Tab overview page [0..PageCount-1]
    int _tabOverviewPage = 0;

    // Pointer to mediator
    CefMediator* _pCefMediator;

    // Bool to remind it should be switched to settings
    bool _goToSettings = false;

    // List of jobs which have to be executed
    std::stack<std::unique_ptr<TabJob> > _jobs;

	// Bookmark manager
	std::unique_ptr<BookmarkManager> _upBookmarkManager;

	// History manager
	std::unique_ptr<HistoryManager> _upHistoryManager;

	// History object
	std::unique_ptr<History> _upHistory;

	// URL input object
	std::unique_ptr<URLInput> _upURLInput;
};

#endif // WEB_H_
