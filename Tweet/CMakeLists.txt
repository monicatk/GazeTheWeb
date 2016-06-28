cmake_minimum_required(VERSION 2.8)

# Only generate Debug and Release configuration types
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Setup project
project(GazeTheWeb-Tweet)
set(APPNAME Tweet)

# Set release build as default
set(CMAKE_BUILD_TYPE Release)

# Set to ON to enable eye tracker support
set(USEEYETRACKER OFF CACHE BOOL "Use SMI REDn eyetracker as input.")
set(USETOBII OFF CACHE BOOL "If eyetracker used, use Tobii EyeX.")

# Activate C++11 in GCC and CLANG
if(NOT MSVC)
	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
endif(NOT MSVC)

# Used to find content within C++ code
set(CONTENT_PATH ${CMAKE_SOURCE_DIR}/content CACHE PATH "Path to content folder.")
add_definitions(-DCONTENT_PATH="${CONTENT_PATH}")

# Add include path to directory
include_directories(${CMAKE_SOURCE_DIR})

# Collect own code
file(GLOB_RECURSE SOURCES
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/*.c"
)
file(GLOB_RECURSE HEADERS
	"${CMAKE_SOURCE_DIR}/src/*.h"
)

# OpenGLLoader
file(GLOB OGL
	"${CMAKE_SOURCE_DIR}/externals/OGL/*.c")
include_directories("${CMAKE_SOURCE_DIR}/externals/OGL")

# twitCurl
file(GLOB_RECURSE TWITCURL
	"${CMAKE_SOURCE_DIR}/externals/twitCurl/*.cpp"
	"${CMAKE_SOURCE_DIR}/externals/twitCurl/*.h"
)
include_directories("${CMAKE_SOURCE_DIR}/externals/twitcurl")

# Trie
file(GLOB TRIE
	"${CMAKE_SOURCE_DIR}/externals/Trie/*.cpp"
	"${CMAKE_SOURCE_DIR}/externals/Trie/*.h"
)
include_directories("${CMAKE_SOURCE_DIR}/externals/trie")

# Collect all code
set(ALL_CODE
	${SOURCES}
	${HEADERS}
	${OGL}
	${TWITCURL}
	${TRIE})

# Filtering for Visual Studio
if(MSVC)

	# http://stackoverflow.com/questions/9701387/cmake-source-group-multiple-files
	foreach(f ${ALL_CODE})
		# Get the path of the file relative to ${CMAKE_CURRENT_SOURCE_DIR},
		# then alter it (not compulsory)
		file(RELATIVE_PATH SRCGR "${CMAKE_CURRENT_SOURCE_DIR}" ${f})
		set(SRCGR "${APPNAME}/${SRCGR}")

		# Extract the folder, ie remove the filename part
		string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

		# Source_group expects \\ (double antislash), not / (slash)
		string(REPLACE / \\ SRCGR ${SRCGR})
		source_group("${SRCGR}" FILES ${f})
	endforeach()

endif(MSVC)

# Find libraries
if(WIN32) # Windows

	# GLFW 3
	set(GLFW3_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/GLFW")
	include_directories("${GLFW3_DIRECTORY}/include")

	if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
		set(GLFW3_LIBRARIES_DIRECTORY "${GLFW3_DIRECTORY}/lib-vc2015_x64")
	else() # x86
		set(GLFW3_LIBRARIES_DIRECTORY "${GLFW3_DIRECTORY}/lib-vc2015_x86")
	endif()

	find_library(GLFW3_STATIC_LIBRARIES
	  NAMES glfw glfw3
	  HINTS "${GLFW3_LIBRARIES_DIRECTORY}"
	  NO_DEFAULT_PATH)

	# Curl
	set(CURL_DIRECTORY "${CMAKE_SOURCE_DIR}/externals/twitcurl/lib/curl")

	if(CMAKE_SIZEOF_VOID_P EQUAL 8) # x64
		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			set(CURL_LIBRARIES_DIRECTORY "${CURL_DIRECTORY}/static-release-x64")
		else()
			set(CURL_LIBRARIES_DIRECTORY "${CURL_DIRECTORY}/static-debug-x64")
		endif()
	else() # x86
		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			set(CURL_LIBRARIES_DIRECTORY "${CURL_DIRECTORY}/static-release-x86")
		else()
			set(CURL_LIBRARIES_DIRECTORY "${CURL_DIRECTORY}/static-debug-x86")
		endif()
	endif()

	find_library(CURL_LIBRARIES
	NAMES libcurl_a libcurl_a_debug
	HINTS "${CURL_LIBRARIES_DIRECTORY}"
	NO_DEFAULT_PATH)

	add_definitions(-DCURL_STATICLIB)

elseif(APPLE) # Apple

	# TODO: No more working since curl is missing (could be added)

	# GLFW3
	set(GLFW3_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/GLFW")
	include_directories("${GLFW3_DIRECTORY}/include")

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(GLFW3_LIBRARIES_DIRECTORY "${GLFW3_DIRECTORY}/lib-xcode_x64_release")
	else()
		set(GLFW3_LIBRARIES_DIRECTORY "${GLFW3_DIRECTORY}/lib-xcode_x64_debug")
	endif()

	find_library(GLFW3_STATIC_LIBRARIES
		NAMES glfw glfw3
		HINTS "${GLFW3_LIBRARIES_DIRECTORY}"
		NO_DEFAULT_PATH)

	# Apple libraries
	find_library(COCOA_LIBRARY Cocoa)
	find_library(IOKIT_LIBRARY IOKit)
	find_library(CORE_VIDEO_LIBRARY CoreVideo)
	set(APPLE_LIBRARIES ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${CORE_VIDEO_LIBRARY})

else() # Linux, GLFW3 has to be installed via package manager

	# GLFW 3
	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLFW3 REQUIRED glfw3)
	include_directories(${GLFW3_INCLUDE_DIR})

	# Curl
	# ${CURL_LIBRARIES}
	# ${CURL_INCLUDE_DIR}
	find_package(CURL REQUIRED)
	include_directories(${CURL_INCLUDE_DIR}) # Probably two times inlcuded (look at twitcurl folder)

	# STD lib for filesystem
	set(FILESYSTEM_LIBRARIES "stdc++fs")

endif()

# OpenGL (found this way on all systems)
# ${OPENGL_LIBRARIES}
# ${OPENGL_INCLUDE_DIR}
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# eyeGUI (added as subdirectory)
set(EYEGUI_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/eyeGUI-development")
add_subdirectory(${EYEGUI_DIRECTORY})
include_directories("${EYEGUI_DIRECTORY}/include")

if (${USEEYETRACKER})

	if (${USETOBII})
		set(TOBII_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/TobiiEyeXSDK")
		set(TOBII_LIBRARIES_DIRECTORY "${TOBII_DIRECTORY}/lib/x86")
		find_library(TOBII_LIBRARIES
			NAMES Tobii.EyeX.Client
			HINTS "${TOBII_LIBRARIES_DIRECTORY}"
			NO_DEFAULT_PATH)

		include_directories("${TOBII_DIRECTORY}/include")
		add_definitions(-DUSEEYETRACKER_TOBII)

	else() # iViewX

		set(IVIEW_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/externals/iViewX")
		set(IVIEW_LIBRARIES_DIRECTORY "${IVIEW_DIRECTORY}/lib")
		find_library(IVIEW_LIBRARIES
			NAMES iViewXAPI
			HINTS "${IVIEW_LIBRARIES_DIRECTORY}"
			NO_DEFAULT_PATH)

		include_directories("${IVIEW_DIRECTORY}/include")
		add_definitions(-DUSEEYETRACKER_IVIEW)

	endif()
	
endif()

# Creation of executeable
add_executable(${APPNAME} ${ALL_CODE})

# Linking
target_link_libraries(${APPNAME} ${OPENGL_LIBRARIES} ${APPLE_LIBRARIES} ${GLFW3_STATIC_LIBRARIES} eyeGUI ${CURL_LIBRARIES} ${IVIEW_LIBRARIES} ${TOBII_LIBRARIES} ${FILESYSTEM_LIBRARIES})

# Copy dynamic libraries
if(WIN32)

	add_custom_command(TARGET ${APPNAME} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/externals/twitcurl/libeay32.dll"
		$<TARGET_FILE_DIR:${APPNAME}>)
	add_custom_command(TARGET ${APPNAME} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/externals/twitcurl/ssleay32.dll"
		$<TARGET_FILE_DIR:${APPNAME}>)
	add_custom_command(TARGET ${APPNAME} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/externals/TobiiEyeXSDK/lib/x86/Tobii.EyeX.Client.dll"
		$<TARGET_FILE_DIR:${APPNAME}>)

endif(WIN32)

# Remove some variables from CMake GUI
unset(GLFW3_STATIC_LIBRARIES CACHE)
unset(CURL_LIBRARIES CACHE)
unset(IVIEW_LIBRARIES CACHE)
unset(TOBII_LIBRARIES CACHE)