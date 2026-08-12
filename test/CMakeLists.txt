#-------------------------------------------------------------------------------
#	CMAKE SETTINGS
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.25)

#-------------------------------------------------------------------------------
#	PROJECT SETTINGS
#-------------------------------------------------------------------------------

PROJECT(queueTest CXX)

#-------------------------------------------------------------------------------
#	3rd Party
#-------------------------------------------------------------------------------

include(FetchContent)

# Use an installed GTest >= 1.17 if available.
find_package(Threads REQUIRED)
find_package(GTest 1.17 CONFIG QUIET)

if(NOT GTest_FOUND)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
            URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    # Keep GTest's MSVC runtime consistent with the parent project.
    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)
endif()

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
        Threads::Threads
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
