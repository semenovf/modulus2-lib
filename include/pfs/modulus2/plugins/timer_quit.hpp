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
#include "quit.hpp"
#include <condition_variable>
#include <memory>
#include <thread>

namespace modulus {

class timer_quit_plugin : public quit_plugin
{
    std::unique_ptr<std::thread, std::function<void(std::thread*)>> _timer;
    mutable std::condition_variable _cond;
    mutable std::mutex _mtx;
    bool _terminate {false};
    bool _timedout {false};

public:
    timer_quit_plugin (int seconds)
    {
        _timer = std::unique_ptr<std::thread, std::function<void(std::thread *)>>(
              new std::thread([this, seconds] {

                std::unique_lock<std::mutex> locker(_mtx);

                auto reason = _cond.wait_for(locker, std::chrono::seconds(seconds)
                    , [&] {
                        return _terminate;
                    });

                this->quit(); // Emit quit signal

                if (reason == false)
                    _timedout = true; // Timed out
            })
            , [] (std::thread * th) {
                if (th->joinable())
                    th->join();
            });
    }

    ~timer_quit_plugin ()
    {}

    void stop ()
    {
        std::unique_lock<std::mutex> locker(_mtx);
        _terminate = true;
        _cond.notify_all();
    }

    bool timedout () const noexcept
    {
        std::unique_lock<std::mutex> locker(_mtx);
        return _timedout;
    }
};

} // namespace modulus
