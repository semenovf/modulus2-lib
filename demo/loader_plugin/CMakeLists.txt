################################################################################
# Copyright (c) 2021,2022 Vladislav Trifochkin
#
# This file is part of `modulus2-lib`.
#
# Changelog:
#      2021.07.10 Initial version.
################################################################################
project(loader_plugin)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pfs::modulus2)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(m1 SHARED m1.cpp)
add_library(m2 SHARED m2.cpp)
add_library(m3 SHARED m3.cpp)

target_link_libraries(m1 PRIVATE pfs::modulus2)
target_link_libraries(m2 PRIVATE pfs::modulus2)
target_link_libraries(m3 PRIVATE pfs::modulus2)
target_include_directories(m1 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(m2 PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(m3 PRIVATE ${CMAKE_SOURCE_DIR}/include)
