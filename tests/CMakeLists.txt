################################################################################
# Copyright (c) 2021 Vladislav Trifochkin
#
# This file is part of `modulus2-lib` library.
#
# Changelog:
#      2021.05.20 Initial version.
#      2021.12.21 Refactored for using portable_target `ADD_TEST`.
################################################################################
project(modulus2-TESTS CXX C)

list(APPEND TESTS
    modulus2basic
    mangling
    settings)

foreach (t ${TESTS})
    portable_target(ADD_TEST ${t}
        ENABLE_COVERAGE ${DEBBY__ENABLE_COVERAGE}
        SOURCES ${t}.cpp ${${t}_SOURCES}
        LINK pfs::modulus2)
endforeach()
