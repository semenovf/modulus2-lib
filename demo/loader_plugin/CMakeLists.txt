################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
#
# Changelog:
#      2021.07.10 Initial version.
################################################################################
project(loader_plugin)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pfs::modulus2)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

