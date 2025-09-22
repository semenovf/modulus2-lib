////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2025 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.06.18 Initial version
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus/modulus.hpp"
#include "pfs/modulus/iostream_logger.hpp"
#include "pfs/modulus/plugins/platform_quit.hpp"
#include "pfs/modulus/plugins/timer_quit.hpp"
#include <chrono>
#include <thread>
#include <iostream>

using modulus_t = modulus::modulus<modulus::iostream_logger, modulus::null_settings>;

int main ()
{
    using exit_status = modulus_t::exit_status;

    int timeout = 5; // seconds
    modulus::timer_quit_plugin timer_quit_plugin {timeout};
    modulus::platform_quit_plugin quit_plugin;

    modulus_t::dispatcher d{modulus::iostream_logger{}, modulus::null_settings{}};

    d.attach_plugin(timer_quit_plugin);
    d.attach_plugin(quit_plugin);

    std::cout << "Sleeping " << timeout << " second(s) before quit"
        << " (or press Ctrl+C to interrupt at any time) ...\n";

    auto start = std::chrono::steady_clock::now();

    if (d.exec() == exit_status::success) {
        std::cout << "Exit successfully\n";

        if (!timer_quit_plugin.timedout()) {
            std::cout << "Terminated by signal\n";
            timer_quit_plugin.stop();
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Quit signal emitted after "
        << elapsed_seconds.count()
        << " seconds timeout\n";

    return EXIT_SUCCESS;
}
