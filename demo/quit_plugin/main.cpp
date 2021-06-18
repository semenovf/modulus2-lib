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
#include <chrono>
#include <thread>
#include <iostream>

using modulus2 = pfs::modulus2<pfs::iostream_logger>;

class timer_quit_plugin : public pfs::quit_plugin
{
    std::unique_ptr<std::thread, std::function<void(std::thread*)>> _timer;

public:
    timer_quit_plugin (int seconds)
    {
        _timer = std::unique_ptr<std::thread, std::function<void(std::thread *)>>(
              new std::thread([this, seconds] {
                std::cout << "Sleeping " << seconds << " second(s) before quit ...\n";

                auto start = std::chrono::steady_clock::now();
                std::this_thread::sleep_for(std::chrono::seconds(seconds));
                this->quit(); // Emit quit signal
                auto end = std::chrono::steady_clock::now();
                std::chrono::duration<double> elapsed_seconds = end - start;
                std::cout << "Quit signal emitted after "
                    << elapsed_seconds.count()
                    << " seconds timeout\n";
            })
            , [] (std::thread * th) {
                if (th->joinable())
                    th->join();
            });


    }
};

int main ()
{
    using exit_status = modulus2::exit_status;
    pfs::iostream_logger logger;
    modulus2::dispatcher d{logger};
    timer_quit_plugin quit_plugin {2};

    d.attach_plugin(quit_plugin);

    if (d.exec() == exit_status::success) {
        std::cout << "Exit successfully\n";
    }

    return EXIT_SUCCESS;
}
