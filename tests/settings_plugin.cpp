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
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"
#include "pfs/modulus2/plugins/in_memory_settings.hpp"

#if PFS_MODULUS2_LIB__ENABLE_ROCKSDB
#   include "pfs/modulus2/plugins/rocksdb_settings.hpp"
#endif

#include <limits>

using modulus2_type = pfs::modulus::modulus2<pfs::modulus::iostream_logger>;
using namespace pfs::modulus;

class m1 : public modulus2_type::regular_module
{
private:
    bool on_start () override
    {
        auto & s = get_dispatcher().settings();

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

void check (abstract_settings_plugin * settings_ptr)
{
    using exit_status = modulus2_type::exit_status;
    iostream_logger logger;
    modulus2_type::dispatcher d{logger};

    d.attach_plugin(*settings_ptr);

    settings_ptr->set("value.int", std::numeric_limits<std::intmax_t>::max());
    settings_ptr->set("value.uint", std::numeric_limits<std::uintmax_t>::max());
    settings_ptr->set("value.real", std::numeric_limits<double>::max());
    settings_ptr->set("value.string.hello", "Hello");
    settings_ptr->set("value.string.world", std::string{"World"});

    CHECK(d.register_module<m1>(std::make_pair("m1", "")));

    d.exec();
}

TEST_CASE("settings_plugin")
{
    std::vector<abstract_settings_plugin *> settings_list;

    in_memory_settings_plugin in_memory_settings;
    settings_list.push_back(& in_memory_settings);

#if PFS_MODULUS2_LIB__ENABLE_ROCKSDB
    auto rocksdb_path = fs::temp_directory_path() / "modulus2_rocksdb_settings_plugin";

    rocksdb_settings_plugin rocksdb_settings{rocksdb_path};
    REQUIRE(rocksdb_settings.initialized());

    settings_list.push_back(& rocksdb_settings);
#endif

    for (auto settings_ptr: settings_list) {
        check(settings_ptr);
    }

#if PFS_MODULUS2_LIB__ENABLE_ROCKSDB
    fs::remove_all(rocksdb_path);
#endif
}
