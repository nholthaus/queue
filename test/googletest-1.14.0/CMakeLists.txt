if (NOT TARGET gtest)
    # Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
    if (POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
    endif ()

    if (POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    endif ()

    set(gtest_force_shared_crt ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(BUILD_GMOCK OFF)
    set(INSTALL_GTEST OFF)

    include(FetchContent)
    FetchContent_Declare(
		    gtest
            URL ${CMAKE_CURRENT_SOURCE_DIR}/googletest-1.14.0.zip
            URL_HASH MD5=b4911e882c51cba34bebfb5df500a650
    )
    FetchContent_MakeAvailable(gtest)
endif ()