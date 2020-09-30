#-------------------------------------------------------------------------------
#	CMAKE SETTINGS
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)
option(BUILD_TESTS "Build the unit tests" OFF)

#-------------------------------------------------------------------------------
#	PROJECT SETTINGS
#-------------------------------------------------------------------------------

PROJECT(queue CXX)
set(CMAKE_CXX_STANDARD 17)

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

if(MSVC)
    list(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

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

if(BUILD_TESTS)
    enable_testing(true)
    add_subdirectory(test)
endif()