////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.09.12 Initial version.
//      2022.03.14 Refactored.
//      2022.12.22 Implemented as specialization from `debby_settings_plugin`.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "debby_settings.hpp"
#include "pfs/debby/backend/rocksdb/database.hpp"

namespace modulus {

using rocksdb_settings_plugin = debby_settings_plugin<debby::backend::rocksdb::database>;

// class rocksdb_settings_plugin: public abstract_settings_plugin
// {
//     using database_type = debby::keyvalue_database<debby::backend::rocksdb::database>;
//
//     struct property_writer
//     {
//         database_type * pdb;
//         key_type const * pkey;
//
//         void operator () (std::intmax_t x) const
//         {
//             if (x >= (std::numeric_limits<int8_t>::min)() && x <= (std::numeric_limits<int8_t>::max)())
//                 pdb->set(*pkey, static_cast<std::int8_t>(x));
//             else if (x >= (std::numeric_limits<int16_t>::min)() && x <= (std::numeric_limits<int16_t>::max)())
//                 pdb->set(*pkey, static_cast<std::int16_t>(x));
//             else if (x >= (std::numeric_limits<int32_t>::min)() && x <= (std::numeric_limits<int32_t>::max)())
//                 pdb->set(*pkey, static_cast<std::int32_t>(x));
//             else
//                 pdb->set(*pkey, x);
//         }
//
//         void operator () (std::nullptr_t) const
//         {
//             pdb->remove(*pkey);
//         }
//
//         void operator () (bool x) const
//         {
//             // No special variant for bool
//             pdb->set(*pkey, static_cast<std::int8_t>(x));
//         }
//
//         void operator () (double x) const
//         {
//             pdb->set(*pkey, x);
//         }
//
//         void operator () (std::string const & x) const
//         {
//             pdb->set(*pkey, x);
//         }
//     };
//
// private:
//     database_type _db;
//
// private:
//     property fetch_property (key_type const & key
//         , property const & default_value
//         , bool & found) const
//     {
//         found = true;
//         bool ok = true;
//         auto value = _db.fetch(key, & ok);
//
//         if (!ok) {
//             found = false;
//             return default_value;
//         }
//
//         if (pfs::holds_alternative<std::intmax_t>(value)) {
//             // Determine actual type by type of default_value
//             if (pfs::holds_alternative<bool>(default_value))
//                 return property{static_cast<bool>(pfs::get<std::intmax_t>(value))};
//             else
//                 return property{pfs::get<std::intmax_t>(value)};
//         } else if (pfs::holds_alternative<double>(value)) {
//             return property{pfs::get<double>(value)};
//         } else if (pfs::holds_alternative<std::string>(value)) {
//             return property{pfs::get<std::string>(value)};
//         }
//
//         found = false;
//         return default_value;
//     }
//
// protected:
//     property take_property (key_type const & key
//         , property const & default_value) override
//     {
//         bool found;
//         auto prop = fetch_property (key, default_value, found);
//
//         if (!found)
//             set_property(key, prop);
//
//         return prop;
//     }
//
//     property get_property (key_type const & key
//         , property const & default_value) const override
//     {
//         bool found;
//         return fetch_property (key, default_value, found);
//     }
//
//     void set_property (key_type const & key, property const & value) override
//     {
//         pfs::visit(property_writer{& _db, & key}, value);
//     }
//
// public:
//     rocksdb_settings_plugin (pfs::filesystem::path const & db_path)
//         : _db(database_type::make(db_path))
//     {}
// };

} // namespace modulus
