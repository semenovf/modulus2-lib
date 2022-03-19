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
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "settings.hpp"
#include "pfs/filesystem.hpp"
#include "pfs/debby/keyvalue_database.hpp"
#include "pfs/debby/backend/rocksdb/database.hpp"
#include <limits>

namespace modulus {

class rocksdb_settings_plugin: public abstract_settings_plugin
{
    using database_type = debby::keyvalue_database<debby::backend::rocksdb::database>;

    struct property_writer
    {
        database_type * pdb;
        key_type const * pkey;

        void operator () (std::intmax_t x) const
        {
            if (x >= std::numeric_limits<int8_t>::min() && x <= std::numeric_limits<int8_t>::max())
                pdb->set(*pkey, static_cast<std::int8_t>(x));
            else if (x >= std::numeric_limits<int16_t>::min() && x <= std::numeric_limits<int16_t>::max())
                pdb->set(*pkey, static_cast<std::int16_t>(x));
            else if (x >= std::numeric_limits<int32_t>::min() && x <= std::numeric_limits<int32_t>::max())
                pdb->set(*pkey, static_cast<std::int32_t>(x));
            else
                pdb->set(*pkey, x);
        }

        void operator () (std::nullptr_t) const
        {
            pdb->remove(*pkey);
        }

        void operator () (bool x) const
        {
            // No special variant for bool
            pdb->set(*pkey, static_cast<std::int8_t>(x));
        }

        void operator () (double x) const
        {
            pdb->set(*pkey, x);
        }

        void operator () (std::string const & x) const
        {
            pdb->set(*pkey, x);
        }
    };

private:
    database_type _db;

protected:
    property get_property (key_type const & key, property const & default_value) const override
    {
        bool ok = true;
        auto value = _db.fetch(key, & ok);

        if (!ok) {
            this->failure(fmt::format("no property found by key: [{}]", key));
            return default_value;
        }

        if (pfs::holds_alternative<std::intmax_t>(value)) {
            // Determine actual type by type of default_value
            if (pfs::holds_alternative<bool>(default_value))
                return property{static_cast<bool>(pfs::get<std::intmax_t>(value))};
            else
                return property{pfs::get<std::intmax_t>(value)};
        } else if (pfs::holds_alternative<double>(value)) {
            return property{pfs::get<double>(value)};
        } else if (pfs::holds_alternative<std::string>(value)) {
            return property{pfs::get<std::string>(value)};
        }

        return default_value;
    }

    void set_property (key_type const & key, property const & value) override
    {
        pfs::visit(property_writer{& _db, & key}, value);
    }

public:
    rocksdb_settings_plugin (pfs::filesystem::path const & db_path)
        : _db(database_type::make(db_path))
    {}
};

} // namespace modulus
