#-------------------------------------------------------------------------------
#	CMAKE SETTINGS
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
option(BUILD_TESTS "Build the unit tests" OFF)
option(BUILD_DOCUMENTATION "Build the documentation" OFF)

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

#-------------------------------------------------------------------------------
#	DOXYGEN
#-------------------------------------------------------------------------------

if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(WARNING "Doxygen not found, unable to generate documentation")
    else()
        find_package(Git)
        include(gitInfo)
        configure_file(Doxyfile.in ${CMAKE_BINARY_DIR}/Doxyfile @ONLY)
        add_custom_target(documentation COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/Doxyfile"
            WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
            COMMENT "Building the documentation..."
        )
    endif()
endif()