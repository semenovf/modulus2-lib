################################################################################
# Copyright (c) 2021-2025 Vladislav Trifochkin
#
# This file is part of `modulus2-lib`.
#
# Changelog:
#       2021.05.20 Initial version.
#       2021.06.18 Added 'demo' subdirectory.
#       2025.02.18 Up to C++14 standard.
#                  Min CMake version is 3.19 (CMakePresets).
#       2025.09.22 Version 3.0
################################################################################
cmake_minimum_required (VERSION 3.19)
project(modulus-ALL CXX C)

option(MODULUS__BUILD_STRICT "Build with strict policies: C++ standard required, C++ extension is OFF etc" ON)
option(MODULUS__BUILD_TESTS "Build tests" OFF)
option(MODULUS__BUILD_DEMO "Build examples/demo" OFF)
option(MODULUS__DISABLE_FETCH_CONTENT "Disable fetch content if sources of dependencies already exists in the working tree (checks .git subdirectory)" ON)
option(MODULUS__ENABLE_DEBBY "Enable `debby-lib` library for settings backends" ON)

if (MODULUS__BUILD_STRICT)
    if (NOT CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 14)
    endif()

    set(CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF) # use -std=c++XX rather than -std=gnu++XX
endif()

add_subdirectory(2ndparty)
include(${CMAKE_CURRENT_LIST_DIR}/library.cmake)

if (MODULUS__BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()

if (MODULUS__BUILD_DEMO AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/demo)
    add_subdirectory(demo)
endif()

include(GNUInstallDirs)

install(TARGETS modulus
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
