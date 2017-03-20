project(content)

# Collect own code
file(GLOB_RECURSE FILES
	"*")
	
# Add target for content
add_custom_target(content SOURCES ${FILES})

# Remove this CMakeLists
foreach(ITEM ${FILES})
  if(${ITEM} MATCHES "CMakeLists.txt")
    list(REMOVE_ITEM FILES ${ITEM})
  endif()
endforeach()

# Filtering for Visual Studio
if(MSVC)

	# http://stackoverflow.com/questions/9701387/cmake-source-group-multiple-files
	foreach(f ${FILES})
		# Get the path of the file relative to ${CMAKE_CURRENT_SOURCE_DIR},
		# then alter it (not compulsory)
		file(RELATIVE_PATH SRCGR "${CMAKE_CURRENT_SOURCE_DIR}" ${f})
		set(SRCGR "${SRCGR}")

		# Extract the folder, ie remove the filename part
		string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

		# Source_group expects \\ (double antislash), not / (slash)
		string(REPLACE / \\ SRCGR ${SRCGR})
		source_group("${SRCGR}" FILES ${f})
	endforeach()

endif(MSVC)