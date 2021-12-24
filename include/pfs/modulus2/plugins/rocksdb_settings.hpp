////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.09.12 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "settings.hpp"
#include "pfs/filesystem.hpp"
#include "pfs/debby/rocksdb/database.hpp"
#include <limits>

namespace modulus {

class rocksdb_settings_plugin: public abstract_settings_plugin
{
    struct property_writer
    {
        debby::rocksdb::database * pdb;
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

        void operator () (std::nullptr_t) const        { pdb->remove(*pkey); }
        void operator () (double x) const              { pdb->set(*pkey, x); }
        void operator () (std::string const & x) const { pdb->set(*pkey, x); }
        void operator () (blob_t const & x) const      { pdb->set(*pkey, x); }
    };

    struct property_setter
    {
        property * target;

        void operator () (std::nullptr_t) const        { *target = nullptr; }
        void operator () (std::intmax_t x) const       { *target = x; }
        void operator () (double x) const              { *target = x; }
        void operator () (std::string const & x) const { *target = x; }
        void operator () (blob_t const & x) const      { *target = x; }
    };

private:
    debby::rocksdb::database _db;

protected:
    void set (key_type const & key, property const & prop) override
    {
        pfs::visit(property_writer{& _db, & key}, prop);
    }

    void set (key_type const & key, property && prop) override
    {
        pfs::visit(property_writer{& _db, & key}, prop);
    }

public:
    property get (key_type const & key) override
    {
        property result;
        debby::error err;

        auto val = _db.fetch(key, & err);

        // TODO Need error handling
        if (err)
            return property{nullptr};

        pfs::visit(property_setter{& result}, val);

        return result;
    }

public:
    rocksdb_settings_plugin (pfs::filesystem::path const & db_path
        , bool create_if_missing = true)
        : _db(db_path, create_if_missing)
    {}
};

} // namespace modulus
