################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
#
# Changelog:
#      2021.05.20 Initial version.
################################################################################
cmake_minimum_required (VERSION 3.5)
project(modulus2-lib CXX C)

option(PFS_MODULUS2__ENABLE_SPDLOG "Enable `spdlog` library for logger" OFF)
option(PFS_MODULUS2__ENABLE_ROCKSDB "Enable `RocksDb` library as backend for settings" OFF)

# Set default `spdlog` root path
if (PFS_MODULUS2__ENABLE_SPDLOG AND NOT PFS_MODULUS2__SPDLOG_ROOT)
    set(PFS_MODULUS2__SPDLOG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog")
endif()

# Set default `rocksdb` root path
if (PFS_MODULUS2__ENABLE_ROCKSDB AND NOT PFS_MODULUS2__ROCKSDB_ROOT)
    set(PFS_MODULUS2__ROCKSDB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rocksdb")
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::modulus2 ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE pfs::common)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    target_compile_definitions(${PROJECT_NAME} INTERFACE "-D_POSIX_C_SOURCE=1")
endif()

if (PFS_MODULUS2__ENABLE_ROCKSDB)
    if (NOT TARGET rocksdb)
        find_library(ROCKSDB_LIBRARY rocksdb)

        if (NOT ROCKSDB_LIBRARY AND EXISTS ${PFS_MODULUS2__ROCKSDB_ROOT}/CMakeLists.txt)

            #
            # https://github.com/facebook/rocksdb/blob/main/INSTALL.md
            #
            set(ROCKSDB_BUILD_SHARED ON CACHE BOOL "Build RocksDB as shared")
            set(WITH_GFLAGS OFF CACHE BOOL "Disable 'gflags' dependency for RocksDB")
            set(WITH_TESTS OFF CACHE BOOL "Disable build tests for RocksDB")
            set(WITH_BENCHMARK_TOOLS OFF CACHE BOOL "Disable build benchmarks for RocksDB")
            set(WITH_CORE_TOOLS OFF CACHE BOOL "Disable build core tools for RocksDB")
            set(WITH_TOOLS OFF CACHE BOOL "Disable build tools for RocksDB")
            #set(FAIL_ON_WARNINGS OFF CACHE BOOL "Disable process warnings as errors for RocksDB")

            # Disable error for g++ 11.2.0 (RocksDB v6.25.3)
            # error: ‘hostname_buf’ may be used uninitialized [-Werror=maybe-uninitialized]
            if (CMAKE_COMPILER_IS_GNUCXX)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
            endif()

            add_subdirectory(${PFS_MODULUS2__ROCKSDB_ROOT} rocksdb)
            target_include_directories(${PROJECT_NAME} INTERFACE ${PFS_MODULUS2__ROCKSDB_ROOT}/include)
            set(ROCKSDB_LIBRARY rocksdb)
        endif()
    else()
        set(ROCKSDB_LIBRARY rocksdb)
    endif()

    if (ROCKSDB_LIBRARY)
        target_link_libraries(${PROJECT_NAME} INTERFACE ${ROCKSDB_LIBRARY})
        target_compile_definitions(${PROJECT_NAME} INTERFACE "-DPFS_MODULUS2__ROCKSDB_ENABLED=1")
        message(STATUS "`RocksDB` backend enabled")
    else()
        message(WARNING "Unable to enable `RocksDB` backend")
    endif()
endif()

if (PFS_MODULUS2__ENABLE_SPDLOG)
    if (NOT TARGET spdlog::spdlog)
        add_subdirectory(${PFS_MODULUS2__SPDLOG_ROOT} spdlog)
        target_include_directories(${PROJECT_NAME} INTERFACE ${PFS_MODULUS2__SPDLOG_ROOT}/include)
    endif()

    target_link_libraries(${PROJECT_NAME} INTERFACE spdlog::spdlog)
    target_compile_definitions(${PROJECT_NAME} INTERFACE "-DPFS_MODULUS2__SPDLOG_ENABLED=1")
    message(STATUS "`spdlog` enabled")
endif()
