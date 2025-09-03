################################################################################
# Copyright (c) 2021-2025 Vladislav Trifochkin
#
# This file is part of `modulus2-lib`.
#
# Changelog:
#      2021.05.20 Initial version.
#      2021.11.30 Refactored using `portable-target`.
#      2025.02.18 Removed `portable_target` dependency.
#                 Min CMake version is 3.19.
################################################################################
cmake_minimum_required (VERSION 3.19)
project(modulus2
    VERSION 1.0.0
    DESCRIPTION "Modulus2 C++ library"
    LANGUAGES CXX C)

add_library(modulus2 INTERFACE)
add_library(pfs::modulus2 ALIAS modulus2)

if (MSVC)
    target_compile_definitions(modulus2 INTERFACE _CRT_SECURE_NO_WARNINGS)
endif()

target_include_directories(modulus2 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(modulus2 INTERFACE pfs::common)

if (MODULUS2__ENABLE_DEBBY)
    target_link_libraries(modulus2 INTERFACE pfs::debby)
endif()

if (MODULUS2__ENABLE_SPDLOG)
    target_compile_definitions(modulus2 INTERFACE "MODULUS2__SPDLOG_ENABLED=1")
    target_link_libraries(modulus2 INTERFACE spdlog::spdlog)
endif()
