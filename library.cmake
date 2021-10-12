################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
#
# Changelog:
#      2021.05.20 Initial version
################################################################################
cmake_minimum_required (VERSION 3.5)
project(modulus2-lib CXX C)

option(MODULUS2_ENABLE_ROCKSDB_BACKEND "Enable `RocksDB` backend for settings" OFF)
option(MODULUS2_ENABLE_SPDLOG "Enable `spdlog` library for logger" OFF)

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::modulus2 ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE pfs::common)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    target_compile_definitions(${PROJECT_NAME} INTERFACE "-D_POSIX_C_SOURCE=1")
endif()

if (MODULUS2_ENABLE_ROCKSDB_BACKEND)
    find_library(ROCKSDB_LIBRARY rocksdb)

    if (NOT ROCKSDB_LIBRARY AND EXISTS ${CMAKE_SOURCE_DIR}/3rdparty/rocksdb/CMakeLists.txt)
        #
        # https://github.com/facebook/rocksdb/blob/main/INSTALL.md
        #
        set(ROCKSDB_BUILD_SHARED ON CACHE BOOL "Build RocksDB as shared")
        set(WITH_GFLAGS OFF CACHE BOOL "Disable 'gflags' dependency for RocksDB")
        set(WITH_TESTS OFF CACHE BOOL "Disable build tests for RocksDB")
        set(WITH_BENCHMARK_TOOLS OFF CACHE BOOL "Disable build benchmarks for RocksDB")
        set(WITH_CORE_TOOLS OFF CACHE BOOL "Disable build core tools for RocksDB")
        set(WITH_TOOLS OFF CACHE BOOL "Disable build tools for RocksDB")

        add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/rocksdb)
        target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/3rdparty/rocksdb/include)
        set(ROCKSDB_LIBRARY rocksdb)
    endif()

    if (ROCKSDB_LIBRARY)
        target_link_libraries(${PROJECT_NAME} INTERFACE ${ROCKSDB_LIBRARY})
        target_compile_definitions(${PROJECT_NAME} INTERFACE "-DMODULUS2_ROCKSDB_BACKEND_ENABLED=1")
        message(STATUS "`RocksDB` backend enabled")
    else()
        message(WARNING "Unable to enable `RocksDB` backend")
    endif()
endif()

if (MODULUS2_ENABLE_SPDLOG)
    add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/spdlog)

    target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/3rdparty/spdlog/include)
    target_link_libraries(${PROJECT_NAME} INTERFACE spdlog::spdlog)
    target_compile_definitions(${PROJECT_NAME} INTERFACE "-DMODULUS2_ENABLE_SPDLOG=1")
    message(STATUS "`spdlog` enabled")
endif()
