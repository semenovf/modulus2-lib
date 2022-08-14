////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
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
    s->set("value.string.!", "!");

    d.attach_plugin(*s);

    CHECK(d.register_module<m1>(std::make_pair("m1", "")));

    d.exec();
}

TEST_CASE("settings_plugin")
{
#if MODULUS2__ROCKSDB_ENABLED
    pfs::filesystem::path rocksdb_path;
#endif

    // Enclose to brackets for remove `rocksdb_path` directory at the end.
    {
        std::vector<modulus::abstract_settings_plugin*> settings_list;

        modulus::in_memory_settings_plugin in_memory_settings;
        settings_list.push_back(&in_memory_settings);

#if MODULUS2__ROCKSDB_ENABLED
        rocksdb_path = pfs::filesystem::temp_directory_path()
            / PFS__LITERAL_PATH("modulus2_rocksdb_settings_plugin");

        if (pfs::filesystem::exists(rocksdb_path)) {
            try {
                pfs::filesystem::remove_all(rocksdb_path);
            } catch (pfs::filesystem::filesystem_error ex) {
                REQUIRE_MESSAGE(false, ex.what());
            } catch (...) {
                REQUIRE_MESSAGE(false, "Unhandled exception");
            }
        }

        modulus::rocksdb_settings_plugin rocksdb_settings {rocksdb_path};
        settings_list.push_back(&rocksdb_settings);
#endif

        for (auto& settings : settings_list) {
            check(std::move(settings));
        }
    }

#if MODULUS2__ROCKSDB_ENABLED
    try {
        if (!rocksdb_path.empty())
            pfs::filesystem::remove_all(rocksdb_path);
    } catch (pfs::filesystem::filesystem_error ex) {
        REQUIRE_MESSAGE(false, ex.what());
    } catch (...) {
        REQUIRE_MESSAGE(false, "Unhandled exception");
    }
#endif
}
