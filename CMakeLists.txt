include(cmake/PreventInSourceBuilds.cmake)

project(psdl CXX)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

subdirs(src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug)

##############################
# 	   Generate Doxygen  	 #
#		Documentation.		 #
##############################
set(DOXYGEN_CONFIG_FILE Doxyfile.d)
find_package(Doxygen)
if(DOXYGEN_FOUND)
	if(EXISTS ${PROJECT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE})
		add_custom_target(
			doc
			${DOXYGEN_EXECUTABLE}
			${PROJECT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE}
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
	endif(EXISTS ${PROJECT_SOURCE_DIR}/${DOXYGEN_CONFIG_FILE})
endif(DOXYGEN_FOUND)