################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
#
# Changelog:
#      2021.05.20 Initial version.
#      2021.11.30 Refactored using `portable-target`.
################################################################################
cmake_minimum_required (VERSION 3.11)
project(modulus2-lib CXX C)

option(PFS_MODULUS2__ENABLE_SPDLOG "Enable `spdlog` library for logger" OFF)
option(PFS_MODULUS2__ENABLE_ROCKSDB "Enable `RocksDb` library as backend for settings" OFF)

if (NOT TARGET pfs::common)
    portable_target(INCLUDE_PROJECT
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty/pfs/common/library.cmake)
endif()

if (NOT TARGET pfs::debby)
    if (PFS_MODULUS2__ENABLE_ROCKSDB)
        set(PFS_DEBBY__ENABLE_ROCKSDB ON CACHE INTERNAL "")
        set(PFS_DEBBY__ROCKSDB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rocksdb" CACHE INTERNAL "")
        portable_target(INCLUDE_PROJECT
            ${CMAKE_CURRENT_LIST_DIR}/3rdparty/pfs/debby/library.cmake)
    endif()
endif()

if (PFS_MODULUS2__ENABLE_SPDLOG)
    if (NOT PFS_MODULUS2__SPDLOG_ROOT)
        set(PFS_MODULUS2__SPDLOG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog" CACHE INTERNAL "")
    endif()

    portable_target(INCLUDE_PROJECT
        ${CMAKE_CURRENT_LIST_DIR}/cmake/Spdlog.cmake)
endif()

portable_target(LIBRARY ${PROJECT_NAME} INTERFACE ALIAS pfs::modulus2)
portable_target(INCLUDE_DIRS ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
portable_target(LINK ${PROJECT_NAME}
    INTERFACE pfs::common
    INTERFACE pfs::debby)

if (PFS_MODULUS2__ENABLE_SPDLOG)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "-DPFS_MODULUS2__SPDLOG_ENABLED=1")
    portable_target(LINK ${PROJECT_NAME} INTERFACE spdlog::spdlog)
endif()
