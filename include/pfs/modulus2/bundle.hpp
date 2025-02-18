////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2023.08.30 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include <pfs/unified_value.hpp>
#include <string>
#include <unordered_map>

MODULUS2__NAMESPACE_BEGIN

class bundle
{
public:
    using key_type    = std::string;

private:
    using mapped_type = pfs::unified_value;

private:
    std::unordered_map<key_type, mapped_type> _d;

public:
    bundle () = default;
    bundle (bundle const &) = default;
    bundle (bundle &&) = default;
    bundle & operator = (bundle const &) = default;
    bundle & operator = (bundle &&) = default;
    ~bundle () = default;

public:
    bool empty () const noexcept
    {
        return _d.empty();
    }

    template <typename T>
    void put (key_type const & key, T && x)
    {
        _d.emplace(key, mapped_type{std::forward<T>(x)});
    }

    template <typename T>
    T const * get_if (key_type const & key) const
    {
        auto pos = _d.find(key);

        if (pos == _d.end())
            return nullptr;

        return pfs::get_if<T>(& pos->second);
    }

    template <typename T>
    T get (key_type const & key, T const & default_value) const
    {
        auto ptr = get_if<T>(key);
        return ptr ? *ptr : default_value;
    }
};

MODULUS2__NAMESPACE_END
