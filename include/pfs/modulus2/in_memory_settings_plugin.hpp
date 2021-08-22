////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.08.22 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "settings_plugin.hpp"
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace pfs {
namespace modulus {

class in_memory_settings_plugin: public abstract_settings_plugin
{
    std::map<std::string, property> _s;
    mutable std::mutex _mtx;

public:
    using abstract_settings_plugin::set;
    using abstract_settings_plugin::get;

    void set (property::key_type const & key, property::value_type const & value) override
    {
        std::lock_guard<std::mutex> lock{_mtx};
        auto v = property{value};
        _s[key] = std::move(v);
    }

    void set (property::key_type && key, property::value_type && value) override
    {
        std::lock_guard<std::mutex> lock{_mtx};
        auto v = property{std::move(value)};
        _s[key] = std::move(v);
    }

    property get (property::key_type const & key, property::value_type const & default_value) override
    {
        std::lock_guard<std::mutex> lock{_mtx};

        auto it = _s.find(key);

        if (it != _s.end())
            return it->second;

        return property{default_value};
    }
};

}} // namespace pfs::modulus

