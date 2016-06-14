### CLIENT #####################################################################

set(CEF_TARGET Client)

### PARAMETERS #################################################################

set(DEPLOYMENT OFF CACHE BOOL "Setup for deployment.") # TODO: Set content folder path to "." and copy content to binary output folder. If possible, set to Release build.
set(TOBII_EYEX_SUPPORT OFF CACHE BOOL "Support for Tobii EyeX.")
set(SMI_REDN_SUPPORT OFF CACHE BOOL "Support for SMI Red.")

# Pipe them to C++
if(${DEPLOYMENT})
  add_definitions(-DDEPLOYMENT)
endif()
if(${TOBII_EYEX_SUPPORT})
  add_definitions(-DTOBII_SUPPORT)
endif()
if(${SMI_REDN_SUPPORT})
  add_definitions(-DSMI_SUPPORT)
endif()

### CONTENT ####################################################################

set(CLIENT_CONTENT_PATH ${CMAKE_CURRENT_LIST_DIR}/content CACHE PATH "Project specific path. Set manually if it was not found.")
add_definitions(-DCONTENT_PATH="${CLIENT_CONTENT_PATH}")

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

# Set path to externals folder
set(EXTERNALS_DIR "${CMAKE_CURRENT_LIST_DIR}/externals")

# Add OpenGLLoader
file(GLOB OGL
  "${EXTERNALS_DIR}/OGL/*.c")
include_directories(externals/OGL)

# Add GLM
file(GLOB_RECURSE GLM
  "${EXTERNALS_DIR}/GLM/glm/*.hpp")
include_directories("${EXTERNALS_DIR}/GLM")

# Add spdlog
include_directories("${EXTERNALS_DIR}/spdlog/include")

# Collect all code
set(ALL_CODE
  ${SOURCES}
  ${HEADERS}
  ${OGL}
  ${GLM})

### GLFW, eyeGUI and OpenGL ####################################################

# eyeGUI directory and include
set(EYEGUI_DIRECTORY "${EXTERNALS_DIR}/eyeGUI")
include_directories("${EYEGUI_DIRECTORY}/include")

# Find libraries
if(OS_WINDOWS) # Windows

  # eyeGUI
  set(EYEGUI_LIBRARIES_DEBUG "${EYEGUI_DIRECTORY}/lib-vs2015_x86_debug_MTd/eyeGUI.lib")
  set(EYEGUI_LIBRARIES_RELEASE "${EYEGUI_DIRECTORY}/lib-vs2015_x86_release_MT/eyeGUI.lib")

  # GLFW 3
  set(GLFW3_DIRECTORY "${EXTERNALS_DIR}/GLFW")
  include_directories("${GLFW3_DIRECTORY}/include")
  set(GLFW3_LIBRARIES_DIRECTORY "${GLFW3_DIRECTORY}/lib-vc2015_x86_MT")
  find_library(GLFW3_STATIC_LIBRARIES
    NAMES glfw glfw3
    HINTS "${GLFW3_LIBRARIES_DIRECTORY}"
    NO_DEFAULT_PATH)

  # iViewX
  set(SMI_IVIEW_X_DIRECTORY "${EXTERNALS_DIR}/iViewX")
  include_directories("${SMI_IVIEW_X_DIRECTORY}/include")
  set(SMI_IVIEW_X_LIBRARIES_DIRECTORY "${SMI_IVIEW_X_DIRECTORY}")
  find_library(SMI_IVIEW_X_LIBRARIES
    NAMES iViewXAPI
    HINTS "${SMI_IVIEW_X_LIBRARIES_DIRECTORY}"
    NO_DEFAULT_PATH)

  # TobiiEyeXSDK
  set(TOBII_EYEX_SDK_DIRECTORY "${EXTERNALS_DIR}/TobiiEyeXSDK")
  include_directories("${TOBII_EYEX_SDK_DIRECTORY}/include")
  set(TOBII_EYEX_SDK_LIBRARIES_DIRECTORY "${TOBII_EYEX_SDK_DIRECTORY}/lib/x86")
  find_library(TOBII_EYEX_SDK_LIBRARIES
    NAMES Tobii.EyeX.Client
    HINTS "${TOBII_EYEX_SDK_LIBRARIES_DIRECTORY}"
    NO_DEFAULT_PATH)

elseif(OS_LINUX) # Linux, GLFW3 has to be installed via package manager

  # eyeGUI
  set(EYEGUI_LIBRARIES_DEBUG "${EYEGUI_DIRECTORY}/lib-gcc_x64_debug/libeyeGUI.a")
  set(EYEGUI_LIBRARIES_RELEASE "${EYEGUI_DIRECTORY}/lib-gcc_x64_release/libeyeGUI.a")

  # GLFW 3
  find_package(PkgConfig REQUIRED)
  pkg_search_module(GLFW3 REQUIRED glfw3)
  include_directories(${GLFW3_INCLUDE_DIR})

endif()

# Add eyeGUI to debug and release builds
ADD_LOGICAL_TARGET("eyegui_lib" "${EYEGUI_LIBRARIES_DEBUG}" "${EYEGUI_LIBRARIES_RELEASE}")

# OpenGL (found this way on all systems)
# ${OPENGL_LIBRARIES}
# ${OPENGL_INCLUDE_DIR}
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

### CREATION AND LINKING #######################################################

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

  # Linking of dynamic libraries
  target_link_libraries(
    ${CEF_TARGET}
    ${OPENGL_LIBRARIES}
    ${GLFW3_STATIC_LIBRARIES}
    eyegui_lib
    ${SMI_IVIEW_X_LIBRARIES}
        ${TOBII_EYEX_SDK_LIBRARIES}
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

  # Copy Tobii EyeX library to output folder
  add_custom_command(TARGET ${CEF_TARGET} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${TOBII_EYEX_SDK_LIBRARIES_DIRECTORY}/Tobii.EyeX.Client.dll"
      $<TARGET_FILE_DIR:${CEF_TARGET}>)

elseif(OS_LINUX) # Linux

  # Add custom compiler flags (necessary for spdlog)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions" )

  # Creation of executeable
  add_executable(${CEF_TARGET} ${ALL_CODE})

  # Add dependencies
  add_dependencies(${CEF_TARGET} libcef_dll_wrapper)

  # Linking of dynamic libraries
  target_link_libraries(
    ${CEF_TARGET}
    ${OPENGL_LIBRARIES}
    ${GLFW3_STATIC_LIBRARIES}
    eyegui_lib
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
