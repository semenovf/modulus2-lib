////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.08.22 Initial version.
//      2022.03.14 Refactored.
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
    property get_property (key_type const & key, property const & default_value) const override
    {
        std::lock_guard<std::mutex> lock{_mtx};
        auto it = _s.find(key);

        if (it != _s.end())
            return it->second;

        return default_value;
    }

    void set_property (key_type const & key, property const & value) override
    {
        std::lock_guard<std::mutex> lock{_mtx};
        _s[key] = value;
    }
};

} // namespace modulus
