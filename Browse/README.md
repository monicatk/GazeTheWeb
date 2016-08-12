# GazeTheWeb - Browse

![Logo](media/Logo.png)

Gaze controlled web browser, part of the EU-funded research project MAMEM. There exists a rough prototype for testing purposes in [_Prototype_](Prototype) subfolder and the work in progress implementation of the full featured one in the [_Client_](Client) subfolder. Both will only compile on either Windows with Visual Studio 2015 as 32bit project or on Linux with GCC 5.x as 64bit project for the moment. In addition, your graphics card must support OpenGL 3.3 or higher (f.e. not the case for second generation Intel i-GPUs or lower, at least on Windows).

## Videos
* [Demonstration of prototype](https://www.youtube.com/watch?v=zj1u6QTmk5k)

## News
* [Announcement on official MAMEM page](http://www.mamem.eu/gazetheweb-prototype-for-gaze-controlled-browsing-the-web)
* [First user experience](http://www.mamem.eu/mamem-meets-three-remarkable-women)

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
 * **DO NOT** overwrite the provided CMakeLists.txt, otherwise the Prototype and Client is not found.
5. Folder structure should look like this screenshot:
 * ![Folder structure](media/Folder.png)
5. If prototype should be built too, one has to include its subdirectory in the main CMakeLists, line 532.
6. Create a build folder somewhere and execute CMake to create a project, which can be compiled.
 * CLIENT_SMI_REDN_SUPPORT defines, whether Client should compile with support for SMI iViewX
 * CLIENT_TOBII_EYEX_SUPPORT defines, whether Client should compile with support for Tobii EyeX SDK

## Notes
This project uses the Chromium Embedded Framework. Please visit https://bitbucket.org/chromiumembedded/cef for more information about that project!
