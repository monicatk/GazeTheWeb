//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Manager of history.

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

	// Add page. Time and date is added inside method
	void AddPage(std::string URL, std::string title);

	// Get history (TODO: get history from certain date etc)
	std::vector<Page> GetHistory() const;

private:

	// Load history from hard disk. Returns whether successful
	bool LoadHistory();

	// Vector of pages
	std::vector<Page> _pages;

	// Fullpath to history file
	std::string _fullpathHistory;
};

#endif // HISTORYMANAGER_H_
