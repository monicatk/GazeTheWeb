# GazeTheWeb - Browse: Client
Gaze controlled web browser, part of the EU-funded research project MAMEM.

## Structure
![Structure](media/Structure.png)

## HowTo
Please refer to the Readme in the [parent folder](https://github.com/MAMEM/GazeTheWeb/tree/master/Browse) for details about compiling.

For configuration, edit the lines in _src/Setup.h_:
```C++
static const bool FULLSCREEN = false;
static const int INITIAL_WINDOW_WIDTH = 1280;
static const int INITIAL_WINDOW_HEIGHT = 720;
static const float DURATION_BEFORE_INPUT = 1.f; // wait one second before accepting input
static const bool PAUSED_AT_STARTUP = false;
static const bool ENABLE_WEBGL = false; // only on Windows
static const bool LOG_DEBUG_MESSAGES = false;
static const bool LOG_DEBUG_MESSAGES = true;
static const std::string LAB_STREAM_OUTPUT_NAME = "BrowserOutputStream";
static const std::string LAB_STREAM_OUTPUT_SOURCE_ID = "myuniquesourceid23443";
static const std::string LAB_STREAM_INPUT_NAME = "MiddlewareStream"; // may be set to same value as LAB_STREAM_OUTPUT_NAME to receive own events for debugging purposes
```

## Shortcuts
* ESC: Exit application
* Tab: Toggle pause

## Validation
A file named _log.txt_ is created at binary folder containing information about current and last runs. If anything wents not as expected, one should take a look into it.

## Screenshots
![Duckduckgo](media/Screenshot-A.png)

![Text Input](media/Screenshot-B.png)

![Tab Overview](media/Screenshot-C.png)

## Issues
* Currently not building on Linux

## Dependencies
All necessary dependencies are provided in the _externals_ or _submodules_ folder.
* GLM: http://glm.g-truc.net/0.9.7/index.html (MIT license chosen)
* GLFW3: http://www.glfw.org
* iViewX: Connection to the iViewX SDK, copyright SMI GmbH (http://www.smivision.com)
* TobiiEyeX: Connection to Tobii EyeX SDK, copyright Tobii Technology AB (http://developer.tobii.com/eyex-sdk)
* eyeGUI: https://github.com/raphaelmenges/eyeGUI
  * FreeType 2.6.1: http://www.freetype.org (FreeType license chosen)
* spdlog: https://github.com/gabime/spdlog
* liblsl: https://github.com/sccn/labstreaminglayer
  * Boost: https://github.com/boostorg/boost

## License
>Copyright 2016 Raphael Menges and Daniel Müller

>Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

>		http://www.apache.org/licenses/LICENSE-2.0

>Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
