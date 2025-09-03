################################################################################
# Copyright (c) 2025 Vladislav Trifochkin
#
# This file is part of `netty-lib`.
#
# Changelog:
#       2025.09.03 Initial version
################################################################################
include(FetchContent)

if (MODULUS2__ENABLE_SPDLOG)
    if (NOT TARGET spdlog::spdlog)
        if (MODULUS2__DISABLE_FETCH_CONTENT AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/spdlog/.git)
            add_subdirectory(spdlog 3rdparty/spdlog)
        else()
            set(FETCHCONTENT_UPDATES_DISCONNECTED_SPDLOG ON)
            set(SPDLOG_FMT_EXTERNAL_HO ON)

            message(STATUS "Fetching spdlog ...")
            include(FetchContent)
            FetchContent_Declare(spdlog
                GIT_REPOSITORY https://github.com/gabime/spdlog.git
                GIT_TAG v1.15.2
                SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/spdlog
                SUBBUILD_DIR ${CMAKE_BINARY_DIR}/3rdparty/spdlog)
            FetchContent_MakeAvailable(spdlog)
            message(STATUS "Fetching spdlog complete")
        endif()
    endif()
endif()
