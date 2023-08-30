################################################################################
# Copyright (c) 2021,2022 Vladislav Trifochkin
#
# This file is part of `modulus2-lib`.
#
# Changelog:
#      2021.06.18 Initial version.
################################################################################
add_subdirectory(quit_plugin)
add_subdirectory(loader_plugin)

if (MODULUS2__ENABLE_SPDLOG)
    add_subdirectory(spdlog_logger)
endif()
