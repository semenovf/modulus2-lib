////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.08.22 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/emitter.hpp"
#include "pfs/string_view.hpp"
#include "pfs/type_traits.hpp"
#include "pfs/variant.hpp"
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace modulus {

using blob_t  = std::vector<std::uint8_t>;
using basic_property_t = pfs::variant<
      std::nullptr_t
    , std::intmax_t
    , double
    , blob_t        // bytes sequence
    , std::string>; // utf-8 encoded string

struct property: public basic_property_t
{
    using basic_property_t::basic_property_t;

    template <typename T>
    typename std::enable_if<std::is_same<T, std::nullptr_t>::value, bool>::type
    is () const noexcept
    {
        return pfs::holds_alternative<std::nullptr_t>(*this);
    }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    is () const noexcept
    {
        return pfs::holds_alternative<std::intmax_t>(*this);
    }

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, bool>::type
    is () const noexcept
    {
        return pfs::holds_alternative<double>(*this);
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, bool>::type
    is () const noexcept
    {
        return pfs::holds_alternative<std::string>(*this);
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, blob_t>::value, bool>::type
    is () const noexcept
    {
        return pfs::holds_alternative<blob_t>(*this);
    }

    template <typename T>
    T or_default_warn (T && default_value
        , std::string const & warn_msg
        , std::function<void(std::string const &)> warn_callback)
    {
        if (!is<T>())
            warn_callback(warn_msg);

        return or_default(std::forward<T>(default_value));
    }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value, T>::type
    or_default (T && default_value)
    {
        auto p = pfs::get_if<std::intmax_t>(this);
        return p ? static_cast<T>(*p) : default_value;
    }

    template <typename T>
    typename std::enable_if<(std::is_floating_point<T>::value), double>::type
    or_default (T && default_value)
    {
        auto p = pfs::get_if<double>(this);
        return p ? static_cast<T>(*p) : default_value;
    }

    template <typename T>
    typename std::enable_if<(std::is_same<std::string, pfs::remove_cvref_t<T>>::value), std::string>::type
    or_default (T && default_value)
    {
        auto p = pfs::get_if<std::string>(this);
        return p ? *p : default_value;
    }

    // For string_view
    template <typename T>
    typename std::enable_if<(std::is_same<pfs::string_view, pfs::remove_cvref_t<T>>::value), std::string>::type
    or_default (T && default_value)
    {
        auto p = pfs::get_if<std::string>(this);
        return p ? *p : default_value.to_string();
    }

    std::string or_default (char const * default_value)
    {
        auto p = pfs::get_if<std::string>(this);
        return p ? *p : std::string{default_value};
    }
};

class abstract_settings_plugin
{
public:
    using key_type = std::string;

public:
    pfs::emitter_mt<std::string const &> failure;

protected:
    virtual void set (key_type const & key, property const & value) = 0;
    virtual void set (key_type const & key, property && value) = 0;

public:
    virtual property get (key_type const & key) = 0;

public:
    virtual ~abstract_settings_plugin () {}

    // For signed integral but not bool
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value, void>::type
    set (key_type const & key, T && value)
    {
        set(key, property{static_cast<std::intmax_t>(value)});
    }

    // For floating point
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, void>::type
    set (key_type const & key, T && value)
    {
        set(key, property{static_cast<double>(value)});
    }

    void set (key_type const & key, std::string const & value)
    {
        set(key, property{value});
    }

    void set (key_type const & key, pfs::string_view value)
    {
        set(key, property{value.to_string()});
    }

    void set (key_type const & key, char const * value)
    {
        set(key, property{std::string{value}});
    }

    property operator [] (key_type const & key)
    {
        return get(key);
    }
};

class null_settings_plugin: public abstract_settings_plugin
{
protected:
    void set (key_type const & /*key*/, property const & /*value*/) override {}
    void set (key_type const & /*key*/, property && /*value*/) override {}

public:
    property get (key_type const & /*key*/) override
    {
        return property{nullptr};
    }
};

} // namespace modulus
