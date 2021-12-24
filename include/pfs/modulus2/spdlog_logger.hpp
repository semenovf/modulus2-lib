////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.10.11 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "spdlog/spdlog.h"

namespace modulus {

class spdlog_logger
{
    spdlog::logger * _backend_ptr {nullptr};

protected:
    spdlog_logger () {}

    void set_backend (spdlog::logger * backend)
    {
        _backend_ptr = backend;
    }

public:
    spdlog_logger (spdlog::logger & logger)
        : _backend_ptr(& logger)
    {
        // Set error handler
        spdlog::set_error_handler([] (std::string const & msg) {
            spdlog::get("console")->error("*** LOGGER ERROR ***: {}", msg);
        });

        // See https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
        // Sample output:
        // 27-03-2020 09:53:19.466 +05:00 [debug] Some message
        // spdlog::set_pattern("%d-%m-%Y %H:%M:%S.%e %z [%^%l%$] %v");
        //
        // 27-03-2020 09:53:19.466 [D] Some message
        _backend_ptr->set_pattern("%d-%m-%Y %H:%M:%S.%e [%^%L%$] %v");
        _backend_ptr->set_level(spdlog::level::info);
        _backend_ptr->flush_on(spdlog::level::info);
    }

    ~spdlog_logger () = default;

    spdlog_logger (spdlog_logger const &) = delete;
    spdlog_logger & operator = (spdlog_logger const &) = delete;

    spdlog_logger (spdlog_logger &&) = default;
    spdlog_logger & operator = (spdlog_logger &&) = default;

    void set_level (spdlog::level::level_enum level, bool flash_on = true)
    {
        _backend_ptr->set_level(level);

        if (flash_on)
            _backend_ptr->flush_on(level);
    }

    void set_pattern (std::string const & pattern)
    {
        _backend_ptr->set_pattern(pattern);
    }

    void trace (std::string const & msg)
    {
        _backend_ptr->trace(msg);
    }

    void debug (std::string const & msg)
    {
        _backend_ptr->debug(msg);
    }

    void info (std::string const & msg)
    {
        _backend_ptr->info(msg);
    }

    void warn (std::string const & msg)
    {
        _backend_ptr->warn(msg);
    }

    void error (std::string const & msg)
    {
        _backend_ptr->error(msg);
    }
};

} // namespace modulus
