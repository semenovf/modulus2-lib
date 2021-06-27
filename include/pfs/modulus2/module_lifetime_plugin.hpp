////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.06.27 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <memory>
#include <thread>

namespace pfs {

template <typename StringType>
class module_lifetime_plugin
{
protected:
    virtual void on_module_registered (StringType const &) = 0;
    virtual void on_module_unregistered (StringType const &) = 0;

public:
    module_lifetime_plugin () = default;
    ~module_lifetime_plugin () = default;

    module_lifetime_plugin (module_lifetime_plugin const &) = delete;
    module_lifetime_plugin & operator = (module_lifetime_plugin const &) = delete;
    module_lifetime_plugin (module_lifetime_plugin &&) = delete;
    module_lifetime_plugin & operator = (module_lifetime_plugin &&) = delete;

    void module_registered (StringType const & name)
    {
        on_module_registered(name);
    }

    void module_unregistered (StringType const & name)
    {
        on_module_unregistered(name);
    }
};

} // namespace pfs

