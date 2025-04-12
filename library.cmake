################################################################################
# Copyright (c) 2021-2025 Vladislav Trifochkin
#
# This file is part of `modulus2-lib`.
#
# Changelog:
#      2021.05.20 Initial version.
#      2021.11.30 Refactored using `portable-target`.
#      2025.02.18 Removed `portable_target` dependency.
#                 Min CMake version is 3.19.
################################################################################
cmake_minimum_required (VERSION 3.19)
project(modulus2
    VERSION 1.0.0
    DESCRIPTION "Modulus2 C++ library"
    LANGUAGES CXX C)

option(MODULUS2__ENABLE_SPDLOG "Enable `spdlog` library for logger" OFF)
option(MODULUS2__ENABLE_DEBBY "Enable `debby-lib` library for settings backends" ON)

add_library(modulus2 INTERFACE)
add_library(pfs::modulus2 ALIAS modulus2)

if (MSVC)
    target_compile_definitions(modulus2 INTERFACE _CRT_SECURE_NO_WARNINGS)
endif()

target_include_directories(modulus2 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

if (NOT TARGET pfs::common)
    set(FETCHCONTENT_UPDATES_DISCONNECTED_COMMON ON)
    message(STATUS "Fetching common ...")
    include(FetchContent)
    FetchContent_Declare(common
        GIT_REPOSITORY https://github.com/semenovf/common-lib.git
        GIT_TAG master
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/2ndparty/common
        SUBBUILD_DIR ${CMAKE_BINARY_DIR}/2ndparty/common)
    FetchContent_MakeAvailable(common)
    message(STATUS "Fetching common complete")
endif()

target_link_libraries(modulus2 INTERFACE pfs::common)

if (MODULUS2__ENABLE_DEBBY)
    if (NOT TARGET pfs::debby)
        set(FETCHCONTENT_UPDATES_DISCONNECTED_DEBBY ON)
        message(STATUS "Fetching pfs::debby ...")
        include(FetchContent)
        FetchContent_Declare(debby
            GIT_REPOSITORY https://github.com/semenovf/debby-lib.git
            GIT_TAG master
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/2ndparty/debby
            SUBBUILD_DIR ${CMAKE_BINARY_DIR}/2ndparty/debby)
        FetchContent_MakeAvailable(debby)
        message(STATUS "Fetching pfs::debby complete")
    endif()

    target_link_libraries(modulus2 INTERFACE pfs::debby)
endif()

if (MODULUS2__ENABLE_SPDLOG)
    if (NOT TARGET spdlog::spdlog)
        set(FETCHCONTENT_UPDATES_DISCONNECTED_SPDLOG ON)
        message(STATUS "Fetching spdlog ...")
        include(FetchContent)
        FetchContent_Declare(spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.15.2
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/spdlog
            SUBBUILD_DIR ${CMAKE_BINARY_DIR}/3rdparty/spdlog)

        set(SPDLOG_FMT_EXTERNAL_HO ON CACHE BOOL "Use external fmt header-only library instead of bundled" FORCE)

        # if (CMAKE_COMPILER_IS_GNUCXX)
        #     # For link custom shared libraries with spdlog static library
        #     target_compile_options(spdlog PRIVATE "-fPIC")
        # endif()
        #
        # if (MSVC)
        #     target_compile_options(spdlog PRIVATE "/utf-8")
        # endif()

        FetchContent_MakeAvailable(spdlog)
        message(STATUS "Fetching spdlog complete")
    endif()

    target_compile_definitions(modulus2 INTERFACE "MODULUS2__SPDLOG_ENABLED=1")
    target_link_libraries(modulus2 INTERFACE spdlog::spdlog)
endif()
