////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.08.22 Initial version.
//      2021.09.12 Added RocksDB backend testing.
//      2021.12.21 New settings API.
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/filesystem.hpp"
#include "pfs/memory.hpp"
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"
#include "pfs/modulus2/plugins/in_memory_settings.hpp"
#include <limits>

#if MODULUS2__ROCKSDB_ENABLED
#   include "pfs/modulus2/plugins/rocksdb_settings.hpp"
#endif

using modulus2_t = modulus::modulus2<modulus::iostream_logger>;

class m1 : public modulus2_t::regular_module
{
private:
    bool on_start () override
    {
        auto & s = get_dispatcher().settings();

        CHECK(s.get("value.bool")        .is<bool>());
        CHECK(s.get("value.char")        .is<std::intmax_t>());
        CHECK(s.get("value.int8")        .is<std::intmax_t>());
        CHECK(s.get("value.uint8")       .is<std::uintmax_t>());
        CHECK(s.get("value.int16")       .is<std::intmax_t>());
        CHECK(s.get("value.uint16")      .is<std::uintmax_t>());
        CHECK(s.get("value.int32")       .is<std::intmax_t>());
        CHECK(s.get("value.uint32")      .is<std::uintmax_t>());
        CHECK(s.get("value.int64")       .is<std::intmax_t>());
        CHECK(s.get("value.uint64")      .is<std::uintmax_t>());
        CHECK(s.get("value.float")       .is<double>());
        CHECK(s.get("value.double")      .is<double>());
        CHECK(s.get("value.string.hello").is<std::string>());
        CHECK(s.get("value.string.world").is<std::string>());
        CHECK(s.get("value.string.!")    .is<std::string>());

        CHECK_EQ(s.get("value.bool")        .or_default(false), true);
        CHECK_EQ(s.get("value.char")        .or_default(0), '\x42');
        CHECK_EQ(s.get("value.int8")        .or_default(0), std::numeric_limits<std::int8_t>::min());
        CHECK_EQ(s.get("value.uint8")       .or_default(std::uint8_t{0}), std::numeric_limits<std::uint8_t>::max());
        CHECK_EQ(s.get("value.int16")       .or_default(0), std::numeric_limits<std::int16_t>::min());
        CHECK_EQ(s.get("value.uint16")      .or_default(std::uint16_t{0}), std::numeric_limits<std::uint16_t>::max());
        CHECK_EQ(s.get("value.int32")       .or_default(0), std::numeric_limits<std::int32_t>::min());
        CHECK_EQ(s.get("value.uint32")      .or_default(std::uint32_t{0}), std::numeric_limits<std::uint32_t>::max());
        CHECK_EQ(s.get("value.int64")       .or_default(std::int64_t{0}), std::numeric_limits<std::int64_t>::min());
        CHECK_EQ(s.get("value.uint64")      .or_default(std::uint64_t{0}), std::numeric_limits<std::uint64_t>::max());
        CHECK_EQ(s.get("value.float")       .or_default(float{0.0f}), std::numeric_limits<float>::max());
        CHECK_EQ(s.get("value.double")      .or_default(double{0.0f}), std::numeric_limits<double>::max());
        CHECK_EQ(s.get("value.float")       .or_default(0.0f), std::numeric_limits<float>::max());
        CHECK_EQ(s.get("value.double")      .or_default(double{0.0f}), std::numeric_limits<double>::max());
        CHECK_EQ(s.get("value.string.hello").or_default("xxx"), std::string{"Hello"});
        CHECK_EQ(s.get("value.string.world").or_default("xxx"), std::string{"World"});
        CHECK_EQ(s.get("value.string.!")    .or_default("xxx"), std::string{"!"});

        CHECK(s.get("value.not-set").is<std::nullptr_t>());
        CHECK_EQ(s.get("value.not-set").or_default(true), true);
        CHECK_EQ(s.get("value.int.not-set").or_default(42), 42);
        CHECK_EQ(s.get("value.uint.not-set").or_default(std::uint8_t{42}), 42);

        // Test `or_default_warn`
        std::string not_set_sample {"NOT-SET"};
        std::string not_set;
        CHECK_EQ(s.get("value.not-set").or_default_warn(true
            , not_set_sample
            , [& not_set] (std::string const & msg) {
                not_set = msg;
            }), true);

        REQUIRE_EQ(not_set, not_set_sample);

        // No need to enter into dispatcher's loop
        return false;
    }
};

void check (modulus::abstract_settings_plugin * s)
{
    using exit_status = modulus2_t::exit_status;
    modulus::iostream_logger logger;
    modulus2_t::dispatcher d{logger};

    s->set("value.bool", true);
    s->set("value.char", '\x42');
    s->set("value.int8", std::numeric_limits<std::int8_t>::min());
    s->set("value.uint8", std::numeric_limits<std::uint8_t>::max());
    s->set("value.int16", std::numeric_limits<std::int16_t>::min());
    s->set("value.uint16", std::numeric_limits<std::uint16_t>::max());
    s->set("value.int32", std::numeric_limits<std::int32_t>::min());
    s->set("value.uint32", std::numeric_limits<std::uint32_t>::max());
    s->set("value.int64", std::numeric_limits<std::int64_t>::min());
    s->set("value.uint64", std::numeric_limits<std::uint64_t>::max());
    s->set("value.float", std::numeric_limits<float>::max());
    s->set("value.double", std::numeric_limits<double>::max());
    s->set("value.string.hello", "Hello");
    s->set("value.string.world", std::string{"World"});
    s->set("value.string.!", pfs::string_view{"!"});

    d.attach_plugin(*s);

    CHECK(d.register_module<m1>(std::make_pair("m1", "")));

    d.exec();
}

TEST_CASE("settings_plugin")
{
    std::vector<modulus::abstract_settings_plugin*> settings_list;

    modulus::in_memory_settings_plugin in_memory_settings;
    settings_list.push_back(& in_memory_settings);

#if MODULUS2__ROCKSDB_ENABLED
    auto rocksdb_path = pfs::filesystem::temp_directory_path() / "modulus2_rocksdb_settings_plugin";

    modulus::rocksdb_settings_plugin rocksdb_settings {rocksdb_path};
    settings_list.push_back(& rocksdb_settings);
#endif

    for (auto & settings: settings_list) {
        check(std::move(settings));
    }

#if MODULUS2__ROCKSDB_ENABLED
    pfs::filesystem::remove_all(rocksdb_path);
#endif
}
