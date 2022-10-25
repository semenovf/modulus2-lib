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
project(modulus2 CXX C)

option(MODULUS2__ENABLE_EXCEPTIONS "Enable exceptions for library" ON)
option(MODULUS2__ENABLE_SPDLOG "Enable `spdlog` library for logger" OFF)
option(MODULUS2__ENABLE_ROCKSDB "Enable `RocksDb` library as backend for settings" OFF)

portable_target(ADD_INTERFACE ${PROJECT_NAME} ALIAS pfs::modulus2)
#portable_target(SOURCES ${PROJECT_NAME}
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/modulus2/iostream_logger.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/modulus2/mangling.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/modulus2/modulus2.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/modulus2/spdlog_logger.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/dl_loader.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/in_memory_settings.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/loader.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/module_lifetime.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/platform_quit.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/posix_quit.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/quit.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/rocksdb_settings.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/settings.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/timer_quit.hpp
    #${CMAKE_CURRENT_LIST_DIR}/include/pfs/plugins/win32_quit.hpp)
portable_target(INCLUDE_DIRS ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
portable_target(LINK ${PROJECT_NAME} INTERFACE pfs::common pfs::debby)

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

if (MODULUS2__ENABLE_ROCKSDB)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "MODULUS2__ROCKSDB_ENABLED=1")
endif()

if (MODULUS2__ENABLE_SPDLOG)
    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "MODULUS2__SPDLOG_ENABLED=1")
    portable_target(LINK ${PROJECT_NAME} INTERFACE spdlog::spdlog)
endif()

portable_target(TRANSLATE UPDATE ${PROJECT_NAME}
    NO_SOURCE_LOCATION
    LANGUAGES ru
    SOURCES ${CMAKE_CURRENT_LIST_DIR}/include/pfs/modulus2/modulus2.hpp)
