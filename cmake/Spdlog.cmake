################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# Changelog:
#      2021.12.02 Initial version.
#      2021.12.09 Became the part of modulus2-lib and refactored totally.
################################################################################
cmake_minimum_required (VERSION 3.5)

if (NOT MODULUS2__SPDLOG_ROOT)
    set(MODULUS2__SPDLOG_ROOT "${CMAKE_CURRENT_LIST_DIR}/../3rdparty/spdlog")
endif()

set(SPDLOG_FMT_EXTERNAL_HO ON CACHE BOOL "Use external fmt header-only library instead of bundled" FORCE)

add_subdirectory(${MODULUS2__SPDLOG_ROOT} spdlog)

if (CMAKE_COMPILER_IS_GNUCXX)
    # For link custom shared libraries with spdlog static library
    target_compile_options(spdlog PRIVATE "-fPIC")
endif()
