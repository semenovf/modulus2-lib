################################################################################
# Copyright (c) 2021-2024 Vladislav Trifochkin
#
# Changelog:
#      2021.12.02 Initial version.
#      2021.12.09 Became the part of modulus2-lib and refactored totally.
################################################################################
set(SPDLOG_FMT_EXTERNAL_HO ON CACHE BOOL "Use external fmt header-only library instead of bundled" FORCE)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/spdlog 3rdparty/spdlog)

if (CMAKE_COMPILER_IS_GNUCXX)
    # For link custom shared libraries with spdlog static library
    target_compile_options(spdlog PRIVATE "-fPIC")
endif()

if (MSVC)
    target_compile_options(spdlog PRIVATE "/utf-8")
endif()
