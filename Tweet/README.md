# GazeTheWeb - Tweet

![Logo](media/Logo.png)

Twitter application controlled with gaze, part of the EU-funded research project MAMEM. Developed as part of a research lab supervised by Dr. Chandan Kumar and Raphael Menges. Application builds on Windows with Visual Studio 2015 as 32bit project or on Linux with GCC 5.x as 64bit project.

![Logo](media/Screenshot.png)

## Videos
* [Demonstration](https://www.youtube.com/watch?v=NQQfB7nf3qw)

## Developers
The application was designed and developed by the students who participated in the research lab:
Svenja Neuneier, Dennis Hahn, Caterine Ospina Ocampo, Sergei Diez, Saskia Handabura, Kim Ballmes, Wojciech Kwasnik, Benjamin Stephan, Eike Idczak, Matthias Barde, Annika Wießgügel and Philipp Weber

## HowTo
Use CMake to create a project either for Visual Studio 2015 or a Unix Makefile. The provided CMakeLists.txt should find all necessary dependencies in the _externals_ folder. As default, mouse control is chosen. For eye tracker support, set the CMake variable *USEEYETRACKER* to `ON`. The program tries to connect to a SMI REDn device by default. For connection to a Tobii EyeX device one has to set the variable *USETOBII* to `ON`, in addition.

To activate the console, add "-console" to the call arguments of the application. Does only work on Windows.

## Shortcuts
ESC: Exits application

## Dependencies
For support of your eye tracking device, install the corresponding SDK as listed below:
* iViewX SDK: Connection to the iViewX SDK, copyright SMI GmbH (http://www.smivision.com)
* TobiiEyeX SDK: Connection to Tobii EyeX SDK, copyright Tobii Technology AB (http://developer.tobii.com/eyex-sdk)

Dependencies below are provided in the _externals_ folder:
* GLM: http://glm.g-truc.net/0.9.7/index.html (MIT license chosen)
* GLFW3: http://www.glfw.org
* twitCurl: https://github.com/swatkat/twitcurl
* RapidJSON: https://github.com/miloyip/rapidjson
* Autocomplete with Trie: https://github.com/vivekn/autocomplete
* eyeGUI: https://github.com/raphaelmenges/eyeGUI
  * FreeType 2.6.1: http://www.freetype.org (FreeType license chosen)

## Acknowledgments
* Readme and release prepared by Raphael Menges

## Issues
* Twitter API limits amount of access in a certain time frame
* Profile search does not work
* Crashes when something in own profile is activated (debugging fails)
* Only fixed resolution possible

## License
>Copyright 2016 Svenja Neuneier, Dennis Hahn, Caterine Ospina Ocampo, Sergei Diez, Saskia Handabura, Kim Ballmes, Wojciech Kwasnik, Benjamin Stephan, Eike Idczak, Matthias Barde, Annika Wießgügel, Philipp Weber

>Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

>		http://www.apache.org/licenses/LICENSE-2.0

>Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
