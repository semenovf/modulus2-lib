////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.06.27 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <string>
#include <thread>

namespace modulus {

class module_lifetime_plugin
{
public:
    using string_type = std::string;

protected:
    virtual void on_module_about_to_register (string_type const &) {}
    virtual void on_module_registered (string_type const &) {}
    virtual void on_module_unregistered (string_type const &) {}

public:
    module_lifetime_plugin () = default;
    ~module_lifetime_plugin () = default;

    module_lifetime_plugin (module_lifetime_plugin const &) = delete;
    module_lifetime_plugin & operator = (module_lifetime_plugin const &) = delete;
    module_lifetime_plugin (module_lifetime_plugin &&) = delete;
    module_lifetime_plugin & operator = (module_lifetime_plugin &&) = delete;

    void module_about_to_register (string_type const & name)
    {
        on_module_about_to_register(name);
    }

    void module_registered (string_type const & name)
    {
        on_module_registered(name);
    }

    void module_unregistered (string_type const & name)
    {
        on_module_unregistered(name);
    }
};

} // namespace modulus
