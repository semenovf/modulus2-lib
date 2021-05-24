################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
#
# Changelog:
#      2021.05.20 Initial version
################################################################################
cmake_minimum_required (VERSION 3.5)
project(pfs-modulus2 CXX C)

find_package(Threads REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
add_library(pfs::modulus2 ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE pfs::common Threads::Threads)
