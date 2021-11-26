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

namespace fs = pfs::filesystem;

// See https://github.com/facebook/rocksdb
//     https://github.com/facebook/rocksdb/blob/main/examples/simple_example.cc
class rocksdb_settings_plugin: public abstract_settings_plugin
{
    const int MAJOR_VERSION {1};
    const int MINOR_VERSION {0};

    enum class type_prefix: char {
          bad_type    = '\x0'
        , bool_type   = 'b'
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
    static std::string const & error_prefix ()
    {
        static std::string __result {"rocksdb_settings_plugin"};
        return __result;
    }

    void print_error (std::string const & s)
    {
        auto errstr = fmt::format("{}: {}", error_prefix(), s);

        if (this->log_error.has_detectors())
            this->log_error(errstr);
        else
            std::cerr << errstr << std::endl;
    }

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

    static std::string const & data_namespace ()
    {
        static std::string __result {"data-"};
        return __result;
    }

    static std::string const & control_namespace ()
    {
        static std::string __result {"ctrl-"};
        return __result;
    }

    inline std::string type_prefix_to_string (type_prefix t)
    {
        return std::string(1, static_cast<char>(t));
    }

    inline std::pair<bool, type_prefix> to_type_prefix (char ch)
    {
        switch (ch) {
            case static_cast<char>(type_prefix::string_type):
                return std::make_pair(true, type_prefix::string_type);
            case static_cast<char>(type_prefix::bool_type):
                return std::make_pair(true, type_prefix::bool_type);
            case static_cast<char>(type_prefix::int_type):
                return std::make_pair(true, type_prefix::int_type);
            case static_cast<char>(type_prefix::uint_type):
                return std::make_pair(true, type_prefix::uint_type);
            case static_cast<char>(type_prefix::real_type):
                return std::make_pair(true, type_prefix::real_type);
            default:
                break;
        }

        return std::make_pair(false, type_prefix::bad_type);
    }

    inline std::string make_key (std::string const & ns, std::string const & basename)
    {
        return ns + basename;
    }

    bool write (std::string const & ns
        , std::string const & key_basename
        , type_prefix type
        , std::string const & value)
    {
        auto key = make_key(ns, key_basename);
        auto s = type_prefix_to_string(type) + value;
        auto status = _db->Put(rocksdb::WriteOptions(), key, s);

        if (status.ok())
            return true;

        print_error(fmt::format("failed to store value by key `{}': {}"
            , key
            , status.ToString()));
        return false;
    }

    inline std::pair<type_prefix, std::string> read (std::string const & ns
        , std::string const & key_basename)
    {
        auto key = make_key(ns, key_basename);
        std::string s;
        auto status = _db->Get(rocksdb::ReadOptions(), key, & s);

        if (!status.ok()) {
            if (!status.IsNotFound()) {
                print_error(fmt::format("failed to fetch value by key `{}': {}"
                    , key, status.ToString()));
            }

            return std::make_pair(type_prefix::bad_type, std::string{});
        }

        if (s.empty()) {
            print_error(fmt::format("failed to fetch value by key `{}': incomplete value", key));
            return std::make_pair(type_prefix::bad_type, std::string{});
        }

        auto res = to_type_prefix(s[0]);

        if (! res.first) {
            print_error(fmt::format("failed to fetch value by key `{}': bad value type", key));
            return std::make_pair(type_prefix::bad_type, std::string{});
        }

        return std::make_pair(res.second, std::string{s.begin() + 1, s.end()});
    }

    bool write_version ()
    {
        return write(control_namespace(), "major_version"
                , type_prefix::int_type
                , pack_fixed(MAJOR_VERSION))
            && write(control_namespace(), "minor_version"
                , type_prefix::int_type
                , pack_fixed(MINOR_VERSION));
    }

    bool check_version ()
    {
        auto major_version_res = read(control_namespace(), "major_version");
        auto minor_version_res = read(control_namespace(), "minor_version");

        if (major_version_res.first == type_prefix::int_type
                && minor_version_res.first == type_prefix::int_type) {
            return std::to_string(MAJOR_VERSION) == major_version_res.second
                && std::to_string(MINOR_VERSION) == minor_version_res.second;
        }

        print_error(std::string("failed to fetch version"));
        return false;
    }

    std::pair<type_prefix, std::string> encode (property::value_type const & value)
    {
        if (property::is<std::string>(value))
            return std::make_pair(type_prefix::string_type
                , property::value<std::string>(value));

        if (property::is<property::bool_type>(value))
            return std::make_pair(type_prefix::bool_type
                , pack_fixed(property::value<property::bool_type>(value)));

        if (property::is<property::int_type>(value))
            return std::make_pair(type_prefix::int_type
                , pack_fixed(property::value<property::int_type>(value)));

        if (property::is<property::uint_type>(value))
            return std::make_pair(type_prefix::uint_type
                , pack_fixed(property::value<property::uint_type>(value)));

        if (property::is<property::real_type>(value))
            return std::make_pair(type_prefix::real_type
                , pack_fixed(property::value<property::real_type>(value)));

        // Oops, unexpected!
        assert(false);
        return std::make_pair(type_prefix::bad_type, std::string{});
    }

    property decode (type_prefix type, std::string const & s)
    {
        property result;
        bool ok {true};

        switch (type) {
            case type_prefix::string_type:
                result = property{s};
                break;

            case type_prefix::bool_type: {
                auto value = unpack_fixed<property::bool_type>(s.c_str(), s.size(), ok);
                if (ok)
                    result = property{value};
                break;
            }

            case type_prefix::int_type: {
                auto value = unpack_fixed<property::int_type>(s.c_str(), s.size(), ok);
                if (ok)
                    result = property{value};
                break;
            }

            case type_prefix::uint_type: {
                auto value = unpack_fixed<property::uint_type>(s.c_str(), s.size(), ok);
                if (ok)
                    result = property{value};
                break;
            }

            case type_prefix::real_type: {
                auto value = unpack_fixed<property::real_type>(s.c_str(), s.size(), ok);
                if (ok)
                    result = property{value};
                break;
            }

            default:
                break;
        }

        return result;
    }

public:
    using abstract_settings_plugin::set;
    using abstract_settings_plugin::get;

    rocksdb_settings_plugin () {}

    ~rocksdb_settings_plugin ()
    {
        if (_db)
            delete _db;
    }

    bool initialize (fs::path const & db_path)
    {
        rocksdb::Options options;

        // Optimize RocksDB. This is the easiest way to get RocksDB to perform well.
        options.IncreaseParallelism();
        options.OptimizeLevelStyleCompaction();

        // Aggressively check consistency of the data.
        options.paranoid_checks = true;

        if (fs::exists(db_path)) {
            options.create_if_missing = false;
        } else {
            // Create the DB if it's not already present.
            options.create_if_missing = true;
        }

        // Open DB.
        // `db_path` is the path to a directory containing multiple database files
        rocksdb::Status status = rocksdb::DB::Open(options, db_path, & _db);

        if (!status.ok()) {
            print_error(fmt::format("failed to create/open database `{}': {}"
                , db_path.native()
                , status.ToString()));
            return false;
        }

        // Database just created, write version.
        if (options.create_if_missing) {
            write_version();
        } else {
            if (!check_version()) {
                if (_db)
                    delete _db;
                _db = nullptr;
            }
        }

        return true;
    }

    void set (property::key_type const & key, property::value_type const & value) override
    {
        assert(_db);
        auto encoded_res = encode(value);
        write(data_namespace(), key, encoded_res.first, encoded_res.second);
    }

    property get (property::key_type const & key
        , property::value_type const & default_value) override
    {
        auto res = read(data_namespace(), key);

        if (res.first == type_prefix::bad_type)
            return default_value;

        auto result = decode(res.first, res.second);

        return result.has_value() ? result : property{default_value};
    }
};

}} // namespace pfs::modulus
