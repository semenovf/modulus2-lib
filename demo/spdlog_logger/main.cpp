////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.10.11 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/filesystem.hpp"
#include "pfs/fmt.hpp"
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/spdlog_logger.hpp"
#include "pfs/modulus2/plugins/posix_quit.hpp"

// SPDLOG sink headers
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

#if defined(ANDROID)
#   include "spdlog/sinks/android_sink.h"
#endif

using modulus2_type = pfs::modulus::modulus2<pfs::modulus::spdlog_logger>;
using namespace pfs::modulus;
namespace fs = pfs::filesystem;

class m1 : public modulus2_type::regular_module
{
private:
    bool on_start () override
    {
        log_trace("on_start");
        log_debug("on_start");
        log_info("on_start");
        log_warn("on_start");
        log_error("on_start");

        return true;
    }

    bool on_finish () override
    {
        log_trace("on_finish");
        log_debug("on_finish");
        log_info("on_finish");
        log_warn("on_finish");
        log_error("on_finish");
        return true;
    }

public:
    m1 () {}

    ~m1 () {}
};

int main ()
{
    using exit_status = modulus2_type::exit_status;

    ////////////////////////////////////////////////////////////////////////////
    // Creating loggers with multiple sinks
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        fs::temp_directory_path() / "modulus2_spdlog_logger_demo.log", 23, 59));

#if defined(ANDROID)
    std::string tag = "modulus2_spdlog_logger_demo";
    sinks.push_back(std::make_shared<spdlog::sinks::android_sink_mt>(tag));
#endif
    //
    ////////////////////////////////////////////////////////////////////////////

    spdlog::logger logger_backend {"modulus2_spdlog_logger", begin(sinks), end(sinks)};
    spdlog_logger logger {logger_backend};
    logger.set_level(spdlog::level::trace);

    modulus2_type::dispatcher d{logger};
    posix_quit_plugin posix_quit_plugin;

    d.attach_plugin(posix_quit_plugin);
    d.register_module<m1>(std::make_pair("m1", ""));

    fmt::print("Press Ctrl+C to stop execution\n");

    if (d.exec() != exit_status::success)
        return EXIT_FAILURE;

    fmt::print("Exit successfully\n");
    return EXIT_SUCCESS;
}
