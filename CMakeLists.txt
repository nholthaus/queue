#-------------------------------------------------------------------------------
#	CMAKE SETTINGS
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#-------------------------------------------------------------------------------
#	PROJECT SETTINGS
#-------------------------------------------------------------------------------

project(queue CXX)
set(CMAKE_CXX_STANDARD 20)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
set(PROJECT_DESCRIPTION "A C++${CMAKE_CXX_STANDARD} library of various `${PROJECT_NAME}` containers")
set(PROJECT_VERSION ${GIT_TAG})

#-------------------------------------------------------------------------------
#   MAIN PROJECT
#-------------------------------------------------------------------------------

set(${PROJECT_NAME_UPPER}_IS_MAIN_PROJECT false)
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
	set(${PROJECT_NAME_UPPER}_IS_MAIN_PROJECT true)
endif ()

#-------------------------------------------------------------------------------
#   OPTIONS
#-------------------------------------------------------------------------------

option(${PROJECT_NAME_UPPER}_BUILD_DOCUMENTATION "Build the documentation" ${${PROJECT_NAME_UPPER}_IS_MAIN_PROJECT})
option(${PROJECT_NAME_UPPER}_BUILD_TESTS "Build the unit tests" ${${PROJECT_NAME_UPPER}_IS_MAIN_PROJECT})

if (${${PROJECT_NAME_UPPER}_IS_MAIN_PROJECT})
	include(gitInfo)
	message(STATUS "${PROJECT_NAME} main project: ${${PROJECT_NAME_UPPER}_IS_MAIN_PROJECT}")
	message(STATUS "${PROJECT_NAME} build documentation: ${${PROJECT_NAME_UPPER}_BUILD_DOCUMENTATION}")
	message(STATUS "${PROJECT_NAME} build tests: ${${PROJECT_NAME_UPPER}_BUILD_TESTS}")
	message(STATUS "${PROJECT_NAME} using C++ ISO standard: C++${CMAKE_CXX_STANDARD}")
endif ()

#-------------------------------------------------------------------------------
#	SOURCE CODE
#-------------------------------------------------------------------------------

# .h files
list(APPEND HEADER_FILES
     ${CMAKE_CURRENT_SOURCE_DIR}/include/concurrent_queue.h
     ${CMAKE_CURRENT_SOURCE_DIR}/include/circular_queue.h
     )

# .cpp files
list(APPEND SOURCE_FILES

     )

if (MSVC)
	list(APPEND SOURCE_FILES ${HEADER_FILES})
endif ()

#-------------------------------------------------------------------------------
#	INCLUDE DIRECTORIES
#-------------------------------------------------------------------------------

include_directories(${CMAKE_SOURCE_DIR}/include)

#-------------------------------------------------------------------------------
#	INTERFACE LIBRARY
#-------------------------------------------------------------------------------

# Generate the executable
add_library(${PROJECT_NAME} INTERFACE)

# Use the automatically determined RPATH values
set_target_properties(${PROJECT_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)

#-------------------------------------------------------------------------------
#	UNIT TESTS
#-------------------------------------------------------------------------------

if (${${PROJECT_NAME_UPPER}_BUILD_TESTS})
	enable_testing(true)
	add_subdirectory(test)
endif (${${PROJECT_NAME_UPPER}_BUILD_TESTS})

#-------------------------------------------------------------------------------
#	DOCUMENTATION
#-------------------------------------------------------------------------------
if (${${PROJECT_NAME_UPPER}_BUILD_DOCUMENTATION})
	include(doxygen)
	add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_documentation)
endif (${${PROJECT_NAME_UPPER}_BUILD_DOCUMENTATION})