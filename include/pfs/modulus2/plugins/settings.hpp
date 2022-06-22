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
#include "pfs/emitter.hpp"
#include "pfs/type_traits.hpp"
#include "pfs/variant.hpp"
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace modulus {

using property = pfs::variant<
      std::nullptr_t
    , bool
    , std::intmax_t
    , double
    , std::string>;

class abstract_settings_plugin
{
public:
    using key_type = std::string;

public:
    mutable pfs::emitter_mt<std::string const &> failure_printer;
    mutable pfs::emitter_mt<std::string const &> success_printer;

protected:
    /**
     * Get property by @a key and set to @a default_value if property
     * does not exist.
     */
    virtual property take_property (key_type const & key
        , property const & default_value) = 0;

    /**
     * Get property by @a key.
     */
    virtual property get_property (key_type const & key
        , property const & default_value) const = 0;

    /**
     * Set property by @a key to @a value.
     */
    virtual void set_property (key_type const & key, property const & value) = 0;

public:
    virtual ~abstract_settings_plugin () {}

    /**
     * Used for initial initialization for settings (for initialize default
     * values for example).
     */
    virtual bool initialize () { return true; }

    template <typename T>
    typename std::enable_if<std::is_same<T, bool>::value, void>::type
    set (key_type const & key, T value)
    {
        this->set_property(key, property{value});
    }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value
        && !std::is_same<T, bool>::value, void>::type
    set (key_type const & key, T value)
    {
        this->set_property(key, property{static_cast<std::intmax_t>(value)});
    }

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, void>::type
    set (key_type const & key, T value)
    {
        this->set_property(key, property{static_cast<double>(value)});
    }

    void set (key_type const & key, std::string const & value)
    {
        this->set_property(key, property{value});
    }

    void set (key_type const & key, char const * value, std::size_t len)
    {
        this->set_property(key, property{std::string(value, len)});
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, bool>::value, bool>::type
    get (key_type const & key, bool default_value = false) const
    {
        auto prop = get_property(key, property{default_value});
        return pfs::get<bool>(prop);
    }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value
        && !std::is_same<T, bool>::value, T>::type
    get (key_type const & key, T default_value = 0) const
    {
        auto prop = get_property(key, property{static_cast<std::intmax_t>(default_value)});
        return static_cast<T>(pfs::get<std::intmax_t>(prop));
    }

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    get (key_type const & key, T default_value = 0) const
    {
        auto prop = get_property(key, property{static_cast<double>(default_value)});
        return static_cast<T>(pfs::get<double>(prop));
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, T>::type
    get (key_type const & key, std::string const & default_value = std::string{}) const
    {
        auto prop = get_property(key, property{static_cast<std::string>(default_value)});
        return static_cast<T>(pfs::get<std::string>(prop));
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, bool>::value, bool>::type
    take (key_type const & key, bool default_value = false)
    {
        auto prop = take_property(key, property{default_value});
        return pfs::get<bool>(prop);
    }

    template <typename T>
    typename std::enable_if<std::is_integral<T>::value
        && !std::is_same<T, bool>::value, T>::type
    take (key_type const & key, T default_value = 0)
    {
        auto prop = take_property(key, property{static_cast<std::intmax_t>(default_value)});
        return static_cast<T>(pfs::get<std::intmax_t>(prop));
    }

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, T>::type
    take (key_type const & key, T default_value = 0)
    {
        auto prop = take_property(key, property{static_cast<double>(default_value)});
        return static_cast<T>(pfs::get<double>(prop));
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, std::string>::value, T>::type
    take (key_type const & key, std::string const & default_value = std::string{})
    {
        auto prop = take_property(key, property{static_cast<std::string>(default_value)});
        return static_cast<T>(pfs::get<std::string>(prop));
    }
};

class null_settings_plugin: public abstract_settings_plugin
{
protected:
    property take_property (key_type const & /*key*/
        , property const & default_value) override
    {
        return default_value;
    }

    property get_property (key_type const & /*key*/
        , property const & default_value) const override
    {
        return default_value;
    }

    void set_property (key_type const & /*key*/, property const & /*value*/) override {}
};

} // namespace modulus
