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
option(MODULUS2__ENABLE_DEBBY "Enable `debby-lib` library for settings backends" ON)

portable_target(ADD_INTERFACE ${PROJECT_NAME} ALIAS pfs::modulus2)
portable_target(INCLUDE_DIRS ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
portable_target(LINK ${PROJECT_NAME} INTERFACE pfs::common)

if (NOT TARGET pfs::common)
    portable_target(INCLUDE_PROJECT ${CMAKE_CURRENT_LIST_DIR}/2ndparty/common/library.cmake)
endif()

if (MODULUS2__ENABLE_DEBBY)
    if (NOT TARGET pfs::debby AND NOT TARGET pfs::debby::static)
        portable_target(INCLUDE_PROJECT ${CMAKE_CURRENT_LIST_DIR}/2ndparty/debby/library.cmake)
    endif()

    if (TARGET pfs::debby)
        portable_target(LINK ${PROJECT_NAME} INTERFACE pfs::debby)
    elseif (TARGET pfs::debby::static)
        portable_target(LINK ${PROJECT_NAME} INTERFACE pfs::debby::static)
    endif()
endif()

if (MODULUS2__ENABLE_SPDLOG)
    if (NOT TARGET spdlog)
        portable_target(INCLUDE_PROJECT ${CMAKE_CURRENT_LIST_DIR}/3rdparty/spdlog.cmake)
    endif()

    portable_target(DEFINITIONS ${PROJECT_NAME} INTERFACE "MODULUS2__SPDLOG_ENABLED=1")
    portable_target(LINK ${PROJECT_NAME} INTERFACE spdlog::spdlog)
endif()

portable_target(INCLUDE_DIRS ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
portable_target(LINK ${PROJECT_NAME} INTERFACE pfs::common)

#portable_target(TRANSLATE UPDATE ${PROJECT_NAME}
#    NO_SOURCE_LOCATION
#    LANGUAGES ru
#    SOURCES ${CMAKE_CURRENT_LIST_DIR}/include/pfs/modulus2/modulus2.hpp)
