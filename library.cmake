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

portable_target(LIBRARY ${PROJECT_NAME} INTERFACE ALIAS pfs::modulus2)
portable_target(INCLUDE_DIRS ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
portable_target(LINK ${PROJECT_NAME} INTERFACE pfs::common)

#if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        #OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    #target_compile_definitions(${PROJECT_NAME} INTERFACE "-D_POSIX_C_SOURCE=1")
#endif()

# Set default `spdlog` root path
if (PFS_MODULUS2__ENABLE_SPDLOG AND NOT PFS_SPDLOG__ROOT)
    set(PFS_SPDLOG__ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/spdlog")
endif()

# Set default `rocksdb` root path
if (PFS_MODULUS2__ENABLE_ROCKSDB AND NOT PFS_ROCKSDB__ROOT)
    set(PFS_ROCKSDB__ROOT "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rocksdb")
endif()

if (PFS_MODULUS2__ENABLE_SPDLOG)
    include(FindSpdlog)

    if (PFS_SPDLOG__LIBRARY)
        target_link_libraries(${PROJECT_NAME} INTERFACE ${PFS_SPDLOG__LIBRARY})
        target_compile_definitions(${PROJECT_NAME} INTERFACE "-DPFS_MODULUS2__SPDLOG_ENABLED=1")

        if (PFS_SPDLOG__INCLUDE_DIR)
            target_include_directories(${PROJECT_NAME} INTERFACE ${PFS_SPDLOG__INCLUDE_DIR})
        endif()

        message(STATUS "`spdlog` enabled")
    endif()
endif()

if (PFS_MODULUS2__ENABLE_ROCKSDB)
    include(FindRocksDB)

    if (PFS_ROCKSDB__LIBRARY)
        target_link_libraries(${PROJECT_NAME} INTERFACE ${PFS_ROCKSDB__LIBRARY})
        target_compile_definitions(${PROJECT_NAME} INTERFACE "-DPFS_MODULUS2__ROCKSDB_ENABLED=1")

        if (PFS_ROCKSDB__INCLUDE_DIR)
            target_include_directories(${PROJECT_NAME} INTERFACE ${PFS_ROCKSDB__INCLUDE_DIR})
        endif()

        message(STATUS "`RocksDB` backend enabled")
    endif()
endif()
