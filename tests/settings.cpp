////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2025 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.08.22 Initial version.
//      2021.09.12 Added RocksDB backend testing.
//      2021.12.21 New settings API.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <pfs/filesystem.hpp>
#include <pfs/memory.hpp>
#include <pfs/modulus/modulus.hpp>
#include <pfs/modulus/iostream_logger.hpp>
#include <pfs/debby/in_memory.hpp>
#include <pfs/debby/settings.hpp>
#include <limits>

using settings_t = debby::settings<debby::backend_enum::map_st>;
using modulus_t = modulus::modulus<modulus::iostream_logger, settings_t>;

class m1 : public modulus_t::regular_module
{
private:
    bool on_start () override
    {
        auto & s = settings();
        s.set("value.bool", true);
        s.set("value.char", '\x42');
        s.set("value.int8", std::numeric_limits<std::int8_t>::min());
        s.set("value.uint8", std::numeric_limits<std::uint8_t>::max());
        s.set("value.int16", std::numeric_limits<std::int16_t>::min());
        s.set("value.uint16", std::numeric_limits<std::uint16_t>::max());
        s.set("value.int32", std::numeric_limits<std::int32_t>::min());
        s.set("value.uint32", std::numeric_limits<std::uint32_t>::max());
        s.set("value.int64", std::numeric_limits<std::int64_t>::min());
        s.set("value.uint64", std::numeric_limits<std::uint64_t>::max());
        s.set("value.float", std::numeric_limits<float>::max());
        s.set("value.double", std::numeric_limits<double>::max());
        s.set("value.string.hello", "Hello");
        s.set("value.string.world", std::string{"World"});
        s.set("value.string.!", "!");

        // No need to enter into dispatcher's loop
        return false;
    }

    bool on_finish () override
    {
        auto & s = settings();

        CHECK_EQ(s.get<bool>("value.bool"), true);
        CHECK_EQ(s.get<char>("value.char"), '\x42');
        CHECK_EQ(s.get<std::int8_t>("value.int8"), std::numeric_limits<std::int8_t>::min());
        CHECK_EQ(s.get<std::uint8_t>("value.uint8"), std::numeric_limits<std::uint8_t>::max());
        CHECK_EQ(s.get<std::int16_t>("value.int16"), std::numeric_limits<std::int16_t>::min());
        CHECK_EQ(s.get<std::uint16_t>("value.uint16"), std::numeric_limits<std::uint16_t>::max());
        CHECK_EQ(s.get<std::int32_t>("value.int32"), std::numeric_limits<std::int32_t>::min());
        CHECK_EQ(s.get<std::uint32_t>("value.uint32"), std::numeric_limits<std::uint32_t>::max());
        CHECK_EQ(s.get<std::int64_t>("value.int64"), std::numeric_limits<std::int64_t>::min());
        CHECK_EQ(s.get<std::uint64_t>("value.uint64"), std::numeric_limits<std::uint64_t>::max());
        CHECK_EQ(s.get<float>("value.float"), std::numeric_limits<float>::max());
        CHECK_EQ(s.get<double>("value.double"), std::numeric_limits<double>::max());
        CHECK_EQ(s.get<std::string>("value.string.hello"), std::string{"Hello"});
        CHECK_EQ(s.get<std::string>("value.string.world"), std::string{"World"});
        CHECK_EQ(s.get<std::string>("value.string.!"), std::string{"!"});

        CHECK_EQ(s.get<std::string>("value.not-set", "not-set"), std::string{"not-set"});

        return true;
    }
};

TEST_CASE("settings")
{
    using exit_status = modulus_t::exit_status;
    settings_t settings = settings_t::make();
    modulus_t::dispatcher d{modulus::iostream_logger{}, std::move(settings)};

    CHECK(d.register_module<m1>(std::make_pair("m1", "")));

    d.exec();
}
