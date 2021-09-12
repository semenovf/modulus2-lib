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
#include <rocksdb/rocksdb_namespace.h>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include <iostream>

namespace pfs {
namespace modulus {

#if defined(PFS_NO_STD_FILESYSTEM)
    namespace fs = pfs::filesystem;
#else
    namespace fs = std::filesystem;
#endif

class rocksdb_settings_plugin: public abstract_settings_plugin
{
    enum class type_prefix: char {
          bool_type   = 'b'
        , int_type    = 'i'
        , uint_type   = 'u'
        , real_type   = 'r'
        , string_type = 's'
    };

    template <typename T>
    union fixed_packer
    {
        T value;
        char bytes[sizeof(T)];
    };

private:
    rocksdb::DB * _db {nullptr};

private:
    template <typename T>
    inline std::string pack_fixed (T const & value)
    {
        fixed_packer<T> p;
        p.value = value;
        return std::string(p.bytes, sizeof(T));
    }

    template <typename T>
    inline T unpack_fixed (char const * s, std::size_t count, bool & ok)
    {
        ok = true;

        if (sizeof(T) != count) {
            ok = false;
            return T{};
        }

        fixed_packer<T> p;
        std::memcpy(p.bytes, s, count);
        return p.value;
    }

    static std::string const & error_prefix ()
    {
        static std::string __result {"rocksdb_settings_plugin"};
        return __result;
    }

    property decode (std::string const & s)
    {
        if (s.empty())
            return property{};

        bool ok {false};
        property result;

        switch (*s.begin()) {
            case static_cast<char>(type_prefix::string_type): {
                result = property{std::string(s.c_str() + 1, s.size() - 1)};
                break;
            }
            case static_cast<char>(type_prefix::bool_type): {
                auto value = unpack_fixed<property::bool_type>(s.c_str() + 1, s.size() - 1, ok);
                if (ok)
                    result = property{value};
                break;
            }
            case static_cast<char>(type_prefix::int_type): {
                auto value = unpack_fixed<property::int_type>(s.c_str() + 1, s.size() - 1, ok);
                if (ok)
                    result = property{value};
                break;
            }
            case static_cast<char>(type_prefix::uint_type): {
                auto value = unpack_fixed<property::uint_type>(s.c_str() + 1, s.size() - 1, ok);
                if (ok)
                    result = property{value};
                break;
            }
            case static_cast<char>(type_prefix::real_type): {
                auto value = unpack_fixed<property::real_type>(s.c_str() + 1, s.size() - 1, ok);
                if (ok)
                    result = property{value};
                break;
            }
            default:
                break;
        }

        return result;
    }

    std::string encode (property::value_type const & value)
    {
        std::string result;

        if (property::is<std::string>(value)) {
            result = std::string(1, static_cast<char>(type_prefix::string_type))
                + property::value<std::string>(value);
        } else if (property::is<property::bool_type>(value)) {
            result = std::string(1, static_cast<char>(type_prefix::bool_type))
                + pack_fixed(property::value<property::bool_type>(value));
        } else if (property::is<property::int_type>(value)) {
            result = std::string(1, static_cast<char>(type_prefix::int_type))
                + pack_fixed(property::value<property::int_type>(value));
        } else if (property::is<property::uint_type>(value)) {
            result = std::string(1, static_cast<char>(type_prefix::uint_type))
                + pack_fixed(property::value<property::uint_type>(value));
        } else if (property::is<property::real_type>(value)) {
            result = std::string(1, static_cast<char>(type_prefix::real_type))
                + pack_fixed(property::value<property::real_type>(value));
        }

        assert(!result.empty());

        return result;
    }

public:
    using abstract_settings_plugin::set;
    using abstract_settings_plugin::get;

    rocksdb_settings_plugin (fs::path const & db_path)
    {
        //
        // See https://github.com/facebook/rocksdb/blob/main/examples/simple_example.cc
        //
        rocksdb::Options options;

        // Optimize RocksDB. This is the easiest way to get RocksDB to perform well.
        options.IncreaseParallelism();
        options.OptimizeLevelStyleCompaction();

        // Create the DB if it's not already present.
        options.create_if_missing = true;

        // Open DB.
        // `db_path` is the path to a directory containing multiple database files
        rocksdb::Status status = rocksdb::DB::Open(options, db_path, & _db);

        if (status.ok()) {
            std::cerr << fmt::format("{}: failed to create/open database `{}': {}"
                , error_prefix()
                , db_path.native()
                , status.ToString()) << std::endl;
        }
    }

    ~rocksdb_settings_plugin ()
    {
        if (_db)
            delete _db;
    }

    bool initialized () const noexcept
    {
        return _db != nullptr;
    }

    void set (property::key_type const & key, property::value_type const & value) override
    {
        assert(_db);

        std::string s = encode(value);
        auto status = _db->Put(rocksdb::WriteOptions(), key, s);

        if (!status.ok()) {
            this->log_error(fmt::format("{}: failed to store value by key `{}': {}"
                , error_prefix()
                , key
                , status.ToString()));
        }
    }

    property get (property::key_type const & key
        , property::value_type const & default_value) override
    {
        std::string value;
        auto status = _db->Get(rocksdb::ReadOptions(), key, & value);

        if (status.ok()) {
            property result = decode(value);

            if (result.has_value()) {
                return result;
            } else {
                this->log_error(fmt::format("{}: failed to fetch value by key `{}': bad value"
                    , error_prefix()
                    , key
                    , status.ToString()));
            }
        } else {
            if (!status.IsNotFound()) {
                this->log_error(fmt::format("{}: failed to fetch value by key `{}': {}"
                    , error_prefix()
                    , key
                    , status.ToString()));
            }
        }

        return property{default_value};
    }
};

}} // namespace pfs::modulus


