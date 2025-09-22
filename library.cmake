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
project(modulus
    VERSION 3.0.0
    DESCRIPTION "Modulus C++ library"
    LANGUAGES CXX C)

add_library(modulus INTERFACE)
add_library(pfs::modulus ALIAS modulus)

if (MSVC)
    target_compile_definitions(modulus INTERFACE _CRT_SECURE_NO_WARNINGS)
endif()

target_include_directories(modulus INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(modulus INTERFACE pfs::common)

if (MODULUS__ENABLE_DEBBY)
    target_link_libraries(modulus INTERFACE pfs::debby)
endif()
