################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# Changelog:
#      2021.12.02 Initial version.
#      2021.12.09 Became the part of modulus2-lib and refactored totally.
################################################################################
cmake_minimum_required (VERSION 3.5)

if (NOT MODULUS2__SPDLOG_ROOT)
    set(MODULUS2__SPDLOG_ROOT ${CMAKE_CURRENT_LIST_DIR}/3rdparty/spdlog)
endif()

add_subdirectory(${MODULUS2__SPDLOG_ROOT} spdlog)
