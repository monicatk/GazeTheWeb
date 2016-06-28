//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once
#include "externals/eyeGUI-development/include/eyeGUI.h"
#include <iostream>

class CloseScreenButtons :public eyegui::ButtonListener {

public:

	void virtual hit(eyegui::Layout* close_layout, std::string id);
	void virtual up(eyegui::Layout* close_layout, std::string id);
	void virtual down(eyegui::Layout* close_layout, std::string id);
};