//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Manager of history.
// TODO: limit entry count!!!

#ifndef HISTORYMANAGER_H_
#define HISTORYMANAGER_H_

#include <string>
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

	// Get history (TODO: get history from certain date etc)
	std::vector<Page> GetHistory() const;

private:

	// List of filtered pages which will not be added to history
	const std::vector<std::string> _filterURLs
	{
		"about:blank"
	};

	// Load history from hard disk. Returns whether successful
	bool LoadHistory();

	// Filter pages like about:blank. Returns true when page should be NOT added
	bool FilterPage(Page page) const;

	// Vector of pages
	std::vector<Page> _pages;

	// Fullpath to history file
	std::string _fullpathHistory;
};

#endif // HISTORYMANAGER_H_
