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
#include "quit.hpp"
#include <pfs/emitter.hpp>
#include <algorithm>

#if _MSC_VER
#   define MODULUS2__WIN32_CONSOLE_QUIT_PLUGIN_ENABLED 1
#   include <cassert>
#   include <windows.h>
#endif

MODULUS__NAMESPACE_BEGIN

#if MODULUS2__WIN32_CONSOLE_QUIT_PLUGIN_ENABLED

class win32_quit_plugin : public quit_plugin
{
    static win32_quit_plugin * _instance;

private:
    static BOOL handler (DWORD ctrl) 
    {
        assert(_instance);
        
        switch (ctrl) {
            case CTRL_C_EVENT:
            case CTRL_BREAK_EVENT:
            case CTRL_CLOSE_EVENT:
            case CTRL_LOGOFF_EVENT:
            case CTRL_SHUTDOWN_EVENT:
                _instance->quit(0);
                // Signal is handled
                return TRUE;
            default:
                // Pass signal on to the next handler
                return FALSE;
        }
    }

public:
    win32_quit_plugin ()
    {
        assert(_instance == nullptr);
        _instance = this;

        auto success = SetConsoleCtrlHandler(handler, TRUE);
        assert(success);
    }

    ~win32_quit_plugin ()
    {}
};

win32_quit_plugin* win32_quit_plugin::_instance {nullptr};

#endif // MODULUS2__WIN32_CONSOLE_QUIT_PLUGIN_ENABLED

MODULUS__NAMESPACE_END
