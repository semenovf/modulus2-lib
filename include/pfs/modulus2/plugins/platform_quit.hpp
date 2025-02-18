////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2022.07.05 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../namespace.hpp"
#include "posix_quit.hpp"
#include "win32_quit.hpp"

MODULUS2__NAMESPACE_BEGIN

#if MODULUS2__POSIX_QUIT_PLUGIN_ENABLED
    using platform_quit_plugin = posix_quit_plugin;
#elif MODULUS2__WIN32_CONSOLE_QUIT_PLUGIN_ENABLED
    using platform_quit_plugin = win32_quit_plugin;
#else
    static_error(false, "Unsupported Platform for quit plugin");
#endif

MODULUS2__NAMESPACE_END
