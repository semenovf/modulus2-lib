////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.08.22 Initial version.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"
#include "pfs/modulus2/in_memory_settings_plugin.hpp"
#include <limits>

using modulus2_type = pfs::modulus::modulus2<pfs::modulus::iostream_logger>;
using namespace pfs::modulus;

class m1 : public modulus2_type::regular_module
{
private:
    bool on_start () override
    {
        auto & s = _dispatcher_ptr->settings();

        CHECK_EQ(s.get("value.int", std::intmax_t{0}).to_int()
            , std::numeric_limits<std::intmax_t>::max());

        CHECK_EQ(s.get("value.int", false).to_bool(), true);

        CHECK_EQ(s.get("value.int", std::uintmax_t{0}).to_uint()
            , static_cast<std::uintmax_t>(std::numeric_limits<std::intmax_t>::max()));

        CHECK_EQ(s.get("value.int", double{0}).to_real()
            , static_cast<double>(std::numeric_limits<std::intmax_t>::max()));

        CHECK_EQ(s.get("value.int", std::string{}).to_string()
            , std::to_string(std::numeric_limits<std::intmax_t>::max()));

        ////////////////////////////////////////////////////////////////////////

        CHECK_EQ(s.get("value.uint", std::uintmax_t{0}).to_uint()
            , std::numeric_limits<std::uintmax_t>::max());

        CHECK_EQ(s.get("value.real", double{0}).to_real()
            , std::numeric_limits<double>::max());

        CHECK_EQ(s.get("value.string.hello", std::string{}).to_string(), std::string{"Hello"});
        CHECK_EQ(s.get("value.string.world", std::string{}).to_string(), std::string{"World"});

        // TODO Add cast tests for bool, uint, real and string

        // No need to enter into dispatcher's loop
        return false;
    }
};

TEST_CASE("settings_plugin") {

    using exit_status = modulus2_type::exit_status;
    iostream_logger logger;
    modulus2_type::dispatcher d{logger};

    in_memory_settings_plugin settings;
    d.attach_plugin(settings);

    settings.set("value.int", std::numeric_limits<std::intmax_t>::max());
    settings.set("value.uint", std::numeric_limits<std::uintmax_t>::max());
    settings.set("value.real", std::numeric_limits<double>::max());
    settings.set("value.string.hello", "Hello");
    settings.set("value.string.world", std::string{"World"});

    CHECK(d.register_module<m1>(std::make_pair("m1", "")));

    d.exec();
}
