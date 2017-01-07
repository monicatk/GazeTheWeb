//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Delegated by Web class to do input of URL.

#ifndef URLINPUT_H_
#define URLINPUT_H_

#include "submodules/eyeGUI/include/eyeGUI.h"
#include <string>
#include <vector>

// Forward declaration
class Master;
class BookmarkManager;

class URLInput
{
public:

    // Constructor
    URLInput(Master* pMaster, BookmarkManager const * pBookmarkManager);

    // Destructor
    virtual ~URLInput();

    // Update while active. Returns whether input is finished. URL is empty if aborted
    bool Update();

    // Activate
    void Activate(int tabId);

    // Deactivate
    void Deactivate();

    // Get whether active
    bool IsActive() const { return _active; }

    // Get URL. Is set to an empty string if aborted
    std::string GetURL() const;

    // Get id of tab which URL is currently edited. Returned -1 if not defined
    int GetCurrentTabId() const { return _currentTabId; }

private:

	// Show bookmarks
	void ShowBookmarks();

    // Give listeners full access
    friend class URLKeyboardListener;
    friend class URLButtonListener;

    // Listeners for GUI
    class URLKeyboardListener : public eyegui::KeyboardListener
    {
    public:

        URLKeyboardListener(URLInput* pURLInput) { _pURLInput = pURLInput; }
		void virtual keySelected(eyegui::Layout* pLayout, std::string id) {}
        void virtual keyPressed(eyegui::Layout* pLayout, std::string id, std::u16string value);
        void virtual keyPressed(eyegui::Layout* pLayout, std::string id, std::string value) {}

    private:

        URLInput* _pURLInput;
    };

    class URLButtonListener : public eyegui::ButtonListener
    {
    public:

        URLButtonListener(URLInput* pURLInput) { _pURLInput = pURLInput; }
        void hit(eyegui::Layout* pLayout, std::string id) {}
        void down(eyegui::Layout* pLayout, std::string id);
        void up(eyegui::Layout* pLayout, std::string id) {}

    private:

        URLInput* _pURLInput;
    };

    // Instances of listeners
    std::shared_ptr<URLKeyboardListener> _spURLKeyboardListener;
    std::shared_ptr<URLButtonListener> _spURLButtonListener;

    // Pointer to master
    Master* _pMaster;

	// Pointer to bookmark manager
	BookmarkManager const * _pBookmarkManager;

    // Pointer to layouts
    eyegui::Layout* _pLayout;
	eyegui::Layout* _pBookmarksLayout;

    // Collected url
    std::u16string _collectedURL = u"";

    // Bool whether active
    bool _active = false;

    // Bool whether input is finished
    bool _finished = false;

    // Id of current tab
    int _currentTabId = -1;

	// Copy of bookmarks, obtained at every activation from bookmark manager
	std::vector<std::string> _bookmarks;
};

#endif // URLINPUT_H_
