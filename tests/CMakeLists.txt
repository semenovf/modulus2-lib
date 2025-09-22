################################################################################
# Copyright (c) 2021-2025 Vladislav Trifochkin
#
# This file is part of `modulus2-lib` library.
#
# Changelog:
#       2021.05.20 Initial version.
#       2021.12.21 Refactored for using portable_target `ADD_TEST`.
#       2025.02.18 Removed `portable_target` dependency.
################################################################################
project(modulus-TESTS CXX C)

set(TESTS
    modulus_basic
    mangling
    settings)

foreach (target ${TESTS})
    add_executable(${target} ${target}.cpp)
    target_link_libraries(${target} PRIVATE pfs::modulus)
    add_test(NAME ${target} COMMAND ${target})
endforeach()
