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

option(MODULUS2__ENABLE_EXCEPTIONS "Enable exceptions for library" ON)
option(MODULUS2__ENABLE_SPDLOG "Enable `spdlog` library for logger" OFF)
option(MODULUS2__ENABLE_ROCKSDB "Enable `RocksDb` library as backend for settings" OFF)

if (MODULUS2__ENABLE_EXCEPTIONS)
    set(PFS__ENABLE_EXCEPTIONS ON CACHE INTERNAL "")
    set(DEBBY__ENABLE_EXCEPTIONS ON CACHE INTERNAL "")
endif()

if (NOT TARGET pfs::common)
    portable_target(INCLUDE_PROJECT
        ${CMAKE_CURRENT_LIST_DIR}/3rdparty/pfs/common/library.cmake)
endif()

if (NOT TARGET pfs::debby)
    if (MODULUS2__ENABLE_ROCKSDB)
        set(DEBBY__ENABLE_ROCKSDB ON CACHE INTERNAL "")

        if (NOT DEBBY__ROCKSDB_ROOT)
            if (NOT MODULUS2__ROCKSDB_ROOT)
                set(MODULUS2__ROCKSDB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rocksdb" CACHE INTERNAL "")
            endif()
            set(DEBBY__ROCKSDB_ROOT ${MODULUS2__ROCKSDB_ROOT} CACHE INTERNAL "")
        endif()

        portable_target(INCLUDE_PROJECT
            ${CMAKE_CURRENT_LIST_DIR}/3rdparty/pfs/debby/library.cmake)
        endif()
endif()

if (MODULUS2__ENABLE_SPDLOG)
    if (NOT MODULUS2__SPDLOG_ROOT)
        set(MODULUS2__SPDLOG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog" CACHE INTERNAL "")
    endif()

    portable_target(INCLUDE_PROJECT
        ${CMAKE_CURRENT_LIST_DIR}/cmake/Spdlog.cmake)
endif()

portable_target(LIBRARY ${PROJECT_NAME} INTERFACE ALIAS pfs::modulus2)
portable_target(INCLUDE_DIRS ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
portable_target(LINK ${PROJECT_NAME} pfs::common pfs::debby)

if (MODULUS2__ENABLE_ROCKSDB)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "-DMODULUS2__ROCKSDB_ENABLED=1")
endif()

if (MODULUS2__ENABLE_SPDLOG)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "-DMODULUS2__SPDLOG_ENABLED=1")
    portable_target(LINK ${PROJECT_NAME} INTERFACE spdlog::spdlog)
endif()
