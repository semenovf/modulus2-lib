////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.06.18 Initial version
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"
#include "pfs/modulus2/plugins/posix_quit.hpp"
#include "pfs/modulus2/plugins/timer_quit.hpp"
#include <chrono>
#include <thread>
#include <iostream>

using modulus2_t = modulus::modulus2<modulus::iostream_logger>;

int main ()
{
    using exit_status = modulus2_t::exit_status;

    int timeout = 5; // seconds
    modulus::iostream_logger logger;
    modulus::timer_quit_plugin timer_quit_plugin {timeout};
    modulus::posix_quit_plugin posix_quit_plugin;

    modulus2_t::dispatcher d{logger};

    d.attach_plugin(timer_quit_plugin);
    d.attach_plugin(posix_quit_plugin);

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
