////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.06.27 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/emitter.hpp"
#include "quit.hpp"
#include <algorithm>

#if _POSIX_C_SOURCE
#   define PFS_HAVE_POSIX_SIGNALS 1
#   include <cassert>
#   include <csignal>
#endif

namespace modulus {

enum  {
    // SIGHUP  - Hangup detected on controlling terminal
    // or death of controlling process
      hangup_detected = 0x0001

    // SIGINT  - Interrupt from keyboard
    // SIGQUIT - Quit from keyboard
    , keyboard_quit  = 0x0002

    // SIGILL  - Illegal Instruction
    , illegal_instruction = 0x0004

    // SIGABRT - Abort signal from abort(3)
    , abort_called = 0x0008

    // SIGFPE  - Floating-point exception
    , floating_point_exception  = 0x0010

    // SIGKILL - Kill signal
    , kill_signal = 0x0020

    , all_quit_signals  = hangup_detected
            | keyboard_quit
            | illegal_instruction
            | abort_called
            | floating_point_exception
            | kill_signal
};

class posix_quit_plugin : public quit_plugin
{
    uint16_t _signals;
    static posix_quit_plugin * _instance;

private:
    bool set_quit_handler (void (* handler)(int))
    {
#if PFS_HAVE_POSIX_SIGNALS
        std::vector<int> signums_vector;

        struct sigaction act;
        act.sa_handler = handler;
        act.sa_flags = 0;

        // Block every signal during the handler
        sigfillset(& act.sa_mask);

        if (_signals & hangup_detected)
            signums_vector.push_back(SIGHUP);

        if (_signals & keyboard_quit) {
            signums_vector.push_back(SIGINT);
            signums_vector.push_back(SIGQUIT);
        }

        if (_signals & illegal_instruction)
            signums_vector.push_back(SIGILL);

        if (_signals & abort_called)
            signums_vector.push_back(SIGABRT);

        if (_signals & floating_point_exception)
            signums_vector.push_back(SIGFPE);

        if (_signals & kill_signal)
            signums_vector.push_back(SIGKILL);

        auto success = std::all_of(signums_vector.cbegin(), signums_vector.cend()
            , [& act] (int signum) {
                return sigaction(signum, & act, nullptr) >= 0;
            });

        return success;
#endif
    }

public:
    posix_quit_plugin (uint16_t sigs = all_quit_signals)
        : _signals(sigs)
    {
        assert(_instance == nullptr);
        _instance = this;
        set_quit_handler(& posix_quit_plugin::handler);
    }

    ~posix_quit_plugin ()
    {
#if PFS_HAVE_POSIX_SIGNALS
        set_quit_handler(SIG_DFL);
#endif
    }

    static void handler (int)
    {
        assert(_instance);
        _instance->quit();
    }
};

posix_quit_plugin * posix_quit_plugin::_instance {nullptr};

} // namespace modulus
