### CLIENT #####################################################################

set(CEF_TARGET Client)

### PARAMETERS #################################################################

set(CLIENT_DEPLOYMENT OFF CACHE BOOL "Setup for Deployment.")

if(OS_WINDOWS) # Windows
	set(CLIENT_BUILD_SMI_IVIEWX_PLUGIN OFF CACHE BOOL "Build plugin for SMI iViewX.")
	set(CLIENT_BUILD_TOBII_EYEX_PLUGIN OFF CACHE BOOL "Build plugin for Tobii EyeX.")
endif()

# Pipe some of them to C++
if(${CLIENT_DEPLOYMENT})
	add_definitions(-DDEPLOYMENT)
endif()

if(OS_LINUX) # Linux
	add_definitions(-Doff_t=__off_t)
endif()

### EYETRACKER PLUGINS #########################################################

set(EYETRACKER_PLUGIN_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/plugins/Eyetracker")

# Plugin for SMI iViewX
if(${CLIENT_BUILD_SMI_IVIEWX_PLUGIN})

	# SMI iViewX SDK
	set(SMI_IVIEWX_SDK_DIRECTORY "${EXTERNALS_DIR}/iViewX")
	include_directories("${SMI_IVIEWX_SDK_DIRECTORY}/include")
	set(SMI_IVIEWX_SDK_LIBRARIES "${SMI_IVIEWX_SDK_DIRECTORY}/iViewXAPI.lib")

	# Plugin project
	add_library(
		SMIiViewXPlugin
		MODULE
		${EYETRACKER_PLUGIN_DIRECTORY}/Interface/Eyetracker.h
		${EYETRACKER_PLUGIN_DIRECTORY}/Common/EyetrackerData.h
		${EYETRACKER_PLUGIN_DIRECTORY}/Common/EyetrackerData.cpp
		${EYETRACKER_PLUGIN_DIRECTORY}/SMIiViewX/EyetrackerImpl.cpp)
	
	# Link SDK
	target_link_libraries(SMIiViewXPlugin ${SMI_IVIEWX_SDK_LIBRARIES})

	# Tell user about it
	message("Plugin for SMI iViewX will be built.")
	
endif()

# Plugin for Tobii EyeX
if(${CLIENT_BUILD_TOBII_EYEX_PLUGIN})

	# Tobii EyeX SDK
	set(TOBII_EYEX_SDK_DIRECTORY "${EXTERNALS_DIR}/TobiiEyeXSDK")
	include_directories("${TOBII_EYEX_SDK_DIRECTORY}/include")
	set(TOBII_EYEX_SDK_LIBRARIES "${TOBII_EYEX_SDK_DIRECTORY}/lib/x86/Tobii.EyeX.Client.lib")

	# Plugin project
	add_library(
		TobiiEyeXPlugin
		MODULE
		${EYETRACKER_PLUGIN_DIRECTORY}/Interface/Eyetracker.h
		${EYETRACKER_PLUGIN_DIRECTORY}/Common/EyetrackerData.h
		${EYETRACKER_PLUGIN_DIRECTORY}/Common/EyetrackerData.cpp
		${EYETRACKER_PLUGIN_DIRECTORY}/TobiiEyeX/EyetrackerImpl.cpp)
		
	# Link SDK
	target_link_libraries(TobiiEyeXPlugin ${TOBII_EYEX_SDK_LIBRARIES})
	
	# Tell user about it
	message("Plugin for Tobii EyeX will be built.")
	
	# Copy Tobii EyeX library to output folder
	add_custom_command(TARGET TobiiEyeXPlugin PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${TOBII_EYEX_SDK_DIRECTORY}/lib/x86/Tobii.EyeX.Client.dll"
		$<TARGET_FILE_DIR:TobiiEyeXPlugin>)
		
endif()

### CONTENT ####################################################################

set(CLIENT_CONTENT_PATH ${CMAKE_CURRENT_LIST_DIR}/content CACHE PATH "Path to content folder.")
if(${CLIENT_DEPLOYMENT})
	add_definitions(-DCONTENT_PATH="./content")
else()
	add_definitions(-DCONTENT_PATH="${CLIENT_CONTENT_PATH}")
endif()

### SOURCES ####################################################################

# Include directory
include_directories("${CMAKE_CURRENT_LIST_DIR}")

# Collect own code
file(GLOB_RECURSE SOURCES
	"src/*.js"
	"src/*.cpp"
	"src/*.c")

file(GLOB_RECURSE HEADERS
	"src/*.h")
	
### EXTERNALS ##################################################################

# Set path to externals folder
set(EXTERNALS_DIR "${CMAKE_CURRENT_LIST_DIR}/externals")

# Add OpenGLLoader
file(GLOB OGL
	"${EXTERNALS_DIR}/OGL/*.c")
include_directories(externals/OGL)

# LabStreamingLayer directory and include
set(LIBLSL_DIRECTORY "${EXTERNALS_DIR}/liblsl")
include_directories("${LIBLSL_DIRECTORY}/include")
	
### SUBMODULES #################################################################

# Set path to submodules
set(SUBMODULES_DIR "${CMAKE_CURRENT_LIST_DIR}/submodules")

# eyeGUI
set(EYEGUI_DIRECTORY "${SUBMODULES_DIR}/eyeGUI")
include_directories("${EYEGUI_DIRECTORY}/include")
add_subdirectory(${EYEGUI_DIRECTORY})

# GLFW3
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "GLFW build examples." )
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "GLFW build tests.")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "GLFW build docs.")
set(GLFW_INSTALL OFF CACHE INTERNAL "GLFW install.")
set(GLFW_DOCUMENT_INTERNALS OFF CACHE INTERNAL "GLFW document internals.")
set(GLFW_USE_EGL OFF CACHE INTERNAL "GLFW use EGL.")
set(GLFW_USE_HYBRID_HPG OFF CACHE INTERNAL "GLFW use hybrid HPG.")
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE INTERNAL "MSCV runtime library dll.")
set(LIB_SUFFIX "" CACHE INTERNAL "Suffix of lib.")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "GLFW build shared libs.")
add_subdirectory(${SUBMODULES_DIR}/glfw)

# GLM
include_directories("${SUBMODULES_DIR}/GLM")

# spdlog
include_directories("${SUBMODULES_DIR}/spdlog/include")

# spdlog
include_directories("${SUBMODULES_DIR}/spdlog/include")

# TinyXML2 already included in eyeGUI
	
# Find libraries
if(OS_WINDOWS) # Windows

	# LabStreamingLayer (dynamic linking)
	set(LIBLSL_LIBRARIES "${LIBLSL_DIRECTORY}/lib-vs2015_x86_release/liblsl32.lib")

elseif(OS_LINUX) # Linux

	# LabStreamingLayer (static linking)
	set(LIBLSL_LIBRARIES_DEBUG "${LIBLSL_DIRECTORY}/lib-gcc_x64_debug/liblsl.a")
	set(LIBLSL_LIBRARIES_RELEASE "${LIBLSL_DIRECTORY}/lib-gcc_x64_release/liblsl.a")
	set(LIBLSL_BOOST_LIBRARIES_DEBUG "${LIBLSL_DIRECTORY}/lib-gcc_x64_debug/libboost.a")
	set(LIBLSL_BOOST_LIBRARIES_RELEASE "${LIBLSL_DIRECTORY}/lib-gcc_x64_release/libboost.a")

	# Add liblsl to debug and release builds
	ADD_LOGICAL_TARGET("lsl_lib" "${LIBLSL_LIBRARIES_DEBUG}" "${LIBLSL_LIBRARIES_RELEASE}")
	ADD_LOGICAL_TARGET("lsl_boost_lib" "${LIBLSL_BOOST_LIBRARIES_DEBUG}" "${LIBLSL_BOOST_LIBRARIES_RELEASE}")

endif()

# OpenGL (found this way on all systems)
# ${OPENGL_LIBRARIES}
# ${OPENGL_INCLUDE_DIR}
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

### CREATION AND LINKING #######################################################

# Collect all code
set(ALL_CODE
	${SOURCES}
	${HEADERS}
	${OGL})

# Logical target used to link the libcef library
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory
SET_CEF_TARGET_OUT_DIR()

if(OS_WINDOWS) # Windows

	# TODO: find reason for that console output instead of just not displaying it
	set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "/NODEFAULTLIB:LIBCMT" )

	# Resource file
	set(RESOURCE_FILE "${CMAKE_CURRENT_LIST_DIR}/Resources.rc")

	# Creation of executeable
	add_executable(${CEF_TARGET} WIN32 ${ALL_CODE} ${RESOURCE_FILE})

	# TODO: put resources in extra source group

	# Add dependencies
	add_dependencies(${CEF_TARGET} libcef_dll_wrapper)

	# Linking of libraries
	target_link_libraries(
		${CEF_TARGET}
		${OPENGL_LIBRARIES}
		glfw
		eyeGUI
		${LIBLSL_LIBRARIES}
		libcef_lib
		libcef_dll_wrapper
		${CEF_STANDARD_LIBS})

	if(USE_SANDBOX)
		# Logical target used to link the cef_sandbox library
		ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
		target_link_libraries(${CEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
	endif()

	# Add the custom manifest files to the executable
	ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_TARGET}")

	# Copy binary and resource files to the target output directory
	COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
	COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

	# Copy LabStreamingLayer library to output folder
	add_custom_command(TARGET ${CEF_TARGET} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${LIBLSL_DIRECTORY}/lib-vs2015_x86_release/liblsl32.dll"
		$<TARGET_FILE_DIR:${CEF_TARGET}>)

elseif(OS_LINUX) # Linux

	# Add custom compiler flags (necessary for spdlog)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions" )

	# Creation of executeable
	add_executable(${CEF_TARGET} ${ALL_CODE})

	# Add dependencies
	add_dependencies(${CEF_TARGET} libcef_dll_wrapper)

	# Linking of libraries
	target_link_libraries(
		${CEF_TARGET}
		${OPENGL_LIBRARIES}
		glfw
		eyeGUI
		lsl_lib
		lsl_boost_lib
		libcef_lib
		libcef_dll_wrapper
		${CEF_STANDARD_LIBS})

	# Set rpath so that libraries can be placed next to the executable
	set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
	set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

	# Copy binary and resource files to the target output directory
	COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
	COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

	# Set SUID permissions on the chrome-sandbox target
	SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")

endif()

### FILTERING ##################################################################

# Filtering for Visual Studio
if(MSVC)

	# http://stackoverflow.com/questions/9701387/cmake-source-group-multiple-files
	foreach(f ${ALL_CODE})
		# Get the path of the file relative to ${CMAKE_CURRENT_SOURCE_DIR},
		# then alter it (not compulsory)
		file(RELATIVE_PATH SRCGR "${CMAKE_CURRENT_SOURCE_DIR}" ${f})
		set(SRCGR "${LIBNAME}/${SRCGR}")

		# Extract the folder, ie remove the filename part
		string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

		# Source_group expects \\ (double antislash), not / (slash)
		string(REPLACE / \\ SRCGR ${SRCGR})
		source_group("${SRCGR}" FILES ${f})
	endforeach()

endif(MSVC)

### DEPLOYMENT #################################################################

# Copy content to output folder
if(CLIENT_DEPLOYMENT)
add_custom_command(TARGET ${CEF_TARGET} PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CLIENT_CONTENT_PATH}
	$<TARGET_FILE_DIR:${CEF_TARGET}>/content)
endif(CLIENT_DEPLOYMENT)