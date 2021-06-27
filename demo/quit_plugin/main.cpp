////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.06.18 Initial version
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2.hpp"
#include "pfs/iostream_logger.hpp"
#include "pfs/modulus2/posix_quit_plugin.hpp"
#include "pfs/modulus2/timer_quit_plugin.hpp"
#include <chrono>
#include <thread>
#include <iostream>

using modulus2 = pfs::modulus2<pfs::iostream_logger>;

int main ()
{
    using exit_status = modulus2::exit_status;

    int timeout = 5; // seconds
    pfs::iostream_logger logger;
    modulus2::dispatcher d{logger};
    pfs::timer_quit_plugin timer_quit_plugin {timeout};
    pfs::posix_quit_plugin posix_quit_plugin;

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
