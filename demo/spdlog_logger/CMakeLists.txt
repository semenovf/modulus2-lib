################################################################################
# Copyright (c) 2021,2022 Vladislav Trifochkin
#
# This file is part of `modulus2-lib`.
#
# Changelog:
#      2021.10.11 Initial version.
################################################################################
project(spdlog_logger)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pfs::modulus2)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

