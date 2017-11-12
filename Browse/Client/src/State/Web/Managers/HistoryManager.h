//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Manager of history.

#ifndef HISTORYMANAGER_H_
#define HISTORYMANAGER_H_

#include <string>
#include <deque>
#include <vector>
#include <functional>
#include <memory>

class HistoryManager
{
public:

	class Page : std::enable_shared_from_this<Page>
	{
	public:

		// Constructor
		Page(std::string URL, std::string title, std::function<void(std::shared_ptr<const Page>)> saveCallback)
			: _id(_idCount++), _URL(URL), _title(title), _saveCallback(saveCallback) {}

		// Set title
		void SetTitle(std::string title) { _title = title; _saveCallback(this->shared_from_this()); }

		// Read attributes
		int GetId() const { return _id; }
		std::string GetURL() const { return _URL; }
		std::string GetTitle() const { return _title; }

	private:

		// Members
		int _id;
		std::string _URL;
		std::string _title;
		std::function<void(std::shared_ptr<const Page>)> _saveCallback;
		static int _idCount;
	};

	// Constructor
	HistoryManager(std::string userDirectory);

	// Destructor
	virtual ~HistoryManager();

	// Add page and returns preliminary entry, which can be changed by the callee. Might return nullptr if URL is filtered
	std::shared_ptr<Page> AddPage(std::string URL, std::string title);

	// Get history
	std::shared_ptr<const std::deque<std::shared_ptr<Page> > > GetHistory() const;

private:

	// List of filtered pages which will not be added to history
	static const std::vector<std::string> _filterURLs;

	// Save history to hard disk. Returns whether successful
	bool SavePageInHistory(bool initialStoring, std::shared_ptr<const HistoryManager::Page> spPage);

	// Load history from hard disk. Returns whether successful
	bool LoadHistory();

	// Filter pages like about:blank. Returns true when page should be NOT added
	bool FilterPage(std::string URL) const;

	// Deque of pages
	std::shared_ptr<std::deque<std::shared_ptr<Page> > > _spPages;

	// Fullpath to history file
	std::string _fullpathHistory;
};

#endif // HISTORYMANAGER_H_
