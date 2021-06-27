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

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::modulus2 ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE pfs::common)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    target_compile_definitions(${PROJECT_NAME} INTERFACE "-D_POSIX_C_SOURCE=1")
endif()
