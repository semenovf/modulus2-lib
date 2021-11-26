################################################################################
# Copyright (c) 2019,2020 Vladislav Trifochkin
#
# This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
################################################################################
project(modulus2-lib-TESTS CXX C)

if (PFS_MODULUS2__ENABLE_COVERAGE)
    message(STATUS "Coverage feature enabled")

    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        message(FATAL_ERROR "Coverage feature requires GCC")
    endif()

    set(CMAKE_CXX_FLAGS "-g -O0 --coverage")
    set(CMAKE_C_FLAGS "-g -O0 --coverage")
endif()

set(TEST_NAMES
    modulus2
    mangling
    settings_plugin)

foreach (name ${TEST_NAMES})
    if (${name}_SOURCES)
        add_executable(${name} ${${name}_SOURCES} ${name}.cpp)
    else ()
        add_executable(${name} ${name}.cpp)
    endif()

    # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#Instrumentation-Options
    #
    # --coverage
    #       This option is used to compile and link code instrumented for
    #       coverage analysis. The option is a synonym for -fprofile-arcs
    #       -ftest-coverage (when compiling) and -lgcov (when linking).
    if (PFS_MODULUS2__ENABLE_COVERAGE)
        target_compile_options(${name} PRIVATE -g -O0 --coverage)
        target_link_libraries(${name} PRIVATE -g -O0 --coverage)
    endif()

    target_link_libraries(${name} PRIVATE pfs::modulus2)
    add_test(NAME ${name} COMMAND ${name})
endforeach()

if (PFS_MODULUS2__ENABLE_COVERAGE)
    include(${CMAKE_SOURCE_DIR}/cmake/Coverage.cmake)
    coverage_target("'/usr/*';'*/doctest.h'")
endif()
