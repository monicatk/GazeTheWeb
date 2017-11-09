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

class HistoryManager
{
public:

	struct Page
	{
		std::string URL;
		std::string title;
		// TODO: date and time
	};

	// Constructor
	HistoryManager(std::string userDirectory);

	// Destructor
	virtual ~HistoryManager();

	// Add page
	void AddPage(Page page);

	// Review front history entry
	Page GetFrontEntry() const;

	// Delete last history entry (also in XML file)
	bool DeletePageByUrl(Page page, bool deleteOnlyFirst = false);

	// Get history (TODO: get history from certain date etc)
	std::deque<Page> GetHistory() const;
	std::deque<Page> GetHistory(int count) const;

private:

	// List of filtered pages which will not be added to history
	static const std::vector<std::string> _filterURLs;

	// Load history from hard disk. Returns whether successful
	bool LoadHistory();

	// Filter pages like about:blank. Returns true when page should be NOT added
	bool FilterPage(Page page) const;

	// Deque of pages
	std::deque<Page> _pages;

	// Fullpath to history file
	std::string _fullpathHistory;
};

#endif // HISTORYMANAGER_H_
