################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
#
# Changelog:
#      2021.05.20 Initial version
#      2021.06.18 Added 'demo' subdirectory
################################################################################
cmake_minimum_required (VERSION 3.5)

project(modulus2-lib-ALL CXX)

option(PFS_MODULUS2__BUILD_STRICT "Build with strict policies: C++ standard required, C++ extension is OFF etc" ON)
option(PFS_MODULUS2__BUILD_TESTS "Build tests" OFF)
option(PFS_MODULUS2__BUILD_DEMO "Build examples/demo" OFF)
option(PFS_MODULUS2__ENABLE_COVERAGE "Build tests with coverage support" OFF)

message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

if (PFS_MODULUS2__BUILD_STRICT)
    set(CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF) # use -std=c++XX rather than -std=gnu++XX
endif()

if (NOT TARGET pfs::common)
    include(${CMAKE_CURRENT_LIST_DIR}/3rdparty/pfs/common/library.cmake)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/library.cmake)

if (PFS_MODULUS2__BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()

if (PFS_MODULUS2__BUILD_DEMO AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/demo)
    add_subdirectory(demo)
endif()
