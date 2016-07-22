//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Global constants.

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "submodules/glm/glm/glm.hpp"
#include <string>

static const int EYEGUI_WEB_URLINPUT_LAYER = -1; // in default GUI...
static const int EYEGUI_WEB_LAYER = -2; // in default GUI...
static const int EYEGUI_TAB_LAYER = -3; // in default GUI...
static const int EYEGUI_SUPER_LAYER = 0; // in super GUI...
static const int EYEGUI_CURSOR_LAYER = 1; // in super GUI...
static const int EYEGUI_SETTINGS_LAYER = -2;
static const std::string BLANK_PAGE_URL = "about:blank";
static const int SLOTS_PER_TAB_OVERVIEW_PAGE = 5;
static const int WEB_TAB_OVERVIEW_MINI_PREVIEW_MIP_MAP_LEVEL = 3;
static const int WEB_TAB_OVERVIEW_PREVIEW_MIP_MAP_LEVEL = 0;
static const glm::vec4 TAB_DEFAULT_COLOR_ACCENT = glm::vec4(96.f / 255.f, 125.f / 255.f, 139.f / 255.f, 1.f);
static const int TAB_ACCENT_COLOR_SAMPLING_POINTS = 100;
static const float TAB_SCROLLING_SENSOR_WIDTH = 0.2f;
static const float TAB_SCROLLING_SENSOR_HEIGHT = 0.1f;
static const float TAB_SCROLLING_SENSOR_PADDING = 0.025f;
static const float TAB_GET_PAGE_RES_INTERVAL = 1.0f;
static const float MASTER_PAUSE_ALPHA = 0.35f;
static const float EYEINPUT_MOUSE_OVERRIDE_INIT_FRAME_DURATION = 0.25f; // duration between mouse movement is expected. Triggered by initial movement
static const float EYEINPUT_MOUSE_OVERRIDE_INIT_DISTANCE = 100.f; // pixels on screen (not best but works)
static const float EYEINPUT_MOUSE_OVERRIDE_STOP_DURATION = 3.f; // duration until override is stopped when no mouse movement done
static const int EYETRACKER_SAMPLE_COLLECTION_COUNT = 120;
static const int EYETRACKER_AVERAGE_SAMPLE_COUNT = 5;
static const std::string LOG_FILE_NAME = "log";
static const std::string INTERACTION_FILE_NAME = "interaction";
static const int LOG_FILE_MAX_SIZE = 1024 * 1024;
static const int LOG_FILE_COUNT = 5;
static const float MOUSE_CURSOR_RELATIVE_SIZE = 0.1f;
static const glm::vec3 DOM_TRIGGER_DEBUG_COLOR = glm::vec3(0, 1, 0);
static const glm::vec3 FIXED_ELEMENT_DEBUG_COLOR = glm::vec3(1, 0, 0);
static const float BLUR_FOCUS_RELATIVE_RADIUS = 0.25f; // relative to smaller of both width or height
static const float BLUR_PERIPHERY_MULTIPLIER = 0.7f;

#endif // GLOBAL_H_
