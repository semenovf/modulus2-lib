////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.08.22 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "settings.hpp"
#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace modulus {

class in_memory_settings_plugin: public abstract_settings_plugin
{
    std::map<std::string, property> _s;
    mutable std::mutex _mtx;

protected:
    void set (key_type const & key, property const & prop) override
    {
        std::lock_guard<std::mutex> lock{_mtx};
        _s[key] = prop;
    }

    void set (key_type const & key, property && prop) override
    {
        std::lock_guard<std::mutex> lock{_mtx};
        _s[key] = std::move(prop);
    }

public:
    property get (key_type const & key) override
    {
        std::lock_guard<std::mutex> lock{_mtx};
        auto it = _s.find(key);

        if (it != _s.end())
            return it->second;

        return property{nullptr};
    }
};

} // namespace modulus
