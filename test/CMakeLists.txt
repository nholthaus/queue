#-------------------------------------------------------------------------------
#	CMAKE SETTINGS
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.7)

#-------------------------------------------------------------------------------
#	PROJECT SETTINGS
#-------------------------------------------------------------------------------

PROJECT(queueTest CXX)

#-------------------------------------------------------------------------------
#	3rd Party
#-------------------------------------------------------------------------------

find_package(Threads)
add_subdirectory(googletest-1.14.0)

#-------------------------------------------------------------------------------
#	SOURCE CODE
#-------------------------------------------------------------------------------

# .h files
list(APPEND HEADER_FILES
	circular_queue_test.h
	concurrent_queue_test.h
    )

# .cpp files
list(APPEND SOURCE_FILES
        main.cpp
    )

if(MSVC)
    list(APPEND SOURCE_FILES ${HEADER_FILES})
endif()

#-------------------------------------------------------------------------------
#	INCLUDE DIRECTORIES
#-------------------------------------------------------------------------------

include_directories(${CMAKE_SOURCE_DIR}/include)

#-------------------------------------------------------------------------------
#	ADDITIONAL LIBRARIES
#-------------------------------------------------------------------------------

list(APPEND LIBRARIES
        gtest
        queue
    )

# Remove duplicate library dependencies.
list(REMOVE_DUPLICATES LIBRARIES)

#-------------------------------------------------------------------------------
#	GENERATE TEST EXECUTABLE
#-------------------------------------------------------------------------------

include(GoogleTest)

# Disable deprecation warnings, since those functions were created specifically and only
# to be run by this unit test
add_compile_definitions(_CONCURRENT_QUEUE_NO_WARNINGS)

# Generate the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# link libraries
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

# Use the automatically determined RPATH values
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

gtest_discover_tests(${PROJECT_NAME})

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION tests)