# GazeTheWeb - Browse

![Logo](media/Logo.png)

Gaze controlled web browser, part of the EU-funded research project MAMEM. Currently, a rough prototype exists in the _Prototype_ subfolder, please check the readme [**there**](Prototype) for information about this prototype. Does work on Windows with Visual Studio 2015 as 32bit project or on Linux with GCC 5.x as 64bit project for the moment.

## Videos
* [Demonstration of prototype](https://www.youtube.com/watch?v=zj1u6QTmk5k)

## News
* [Announcement on official MAMEM page](http://www.mamem.eu/gazetheweb-prototype-for-gaze-controlled-browsing-the-web)

## HowTo
Since the CEF3 binaries for Windows and Linux do not like each other, one has to copy them manually into the cloned project. Just follow these easy steps:

1. Clone this repository.
2. Download either Windows 32bit or Linux 64bit CEF 3.x binaries of branch 2454 [**here**](https://cefbuilds.com/#branch_2454).
3. Extract the downloaded files and copy following content into the locally cloned repository:
  * include
  * libcef_dll
  * Release
  * Debug
  * Resources
  * README.txt
  * LICENSE.txt
  * **DO NOT** overwrite the provided CMakeLists.txt, otherwise the prototype is not found.
4. Create a build folder somewhere and execute CMake to create a project, which can be compiled.

## Notes
This project uses the Chromium Embedded Framework. Please visit https://bitbucket.org/chromiumembedded/cef for more information about that project!
