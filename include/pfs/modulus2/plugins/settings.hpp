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
#include "pfs/variant.hpp"
#include <map>
#include <string>
#include <vector>

namespace pfs {
namespace modulus {

#if defined(PFS_NO_STD_VARIANT)
    template <typename... Types>
    using variant_type = pfs::variant<Types...>;
    using ::pfs::get;
    using ::pfs::holds_alternative;
#else
    template <typename... Types>
    using variant_type = std::variant<Types...>;
    using ::std::get;
    using ::std::holds_alternative;
#endif

class property
{
public:
    using bool_type   = bool;
    using int_type    = std::intmax_t;
    using uint_type   = std::uintmax_t;
    using real_type   = double;
    using string_type = std::string;

    using key_type    = string_type;
    using value_type  = variant_type<
          bool_type
        , int_type
        , uint_type
        , real_type
        , string_type>;

private:
    value_type _value;

public:
    property () = default;
    ~property () = default;

    property (property const &) = default;
    property & operator = (property const &) = default;

    property (property &&) = default;
    property & operator = (property &&) = default;

    property (value_type const & value)
        : _value(value)
    {}

    property (value_type && value)
        :_value(std::move(value))
    {}

    bool has_value () const noexcept
    {
        return (_value.valueless_by_exception() == false);
    }

    template <typename T>
    bool is () const
    {
        return holds_alternative<T>(_value);
    }

    template <typename T>
    static bool is (value_type const & value)
    {
        return holds_alternative<T>(value);
    }

    template <typename T>
    static bool is (property const & prop)
    {
        return holds_alternative<T>(prop._value);
    }

    template <typename T>
    T value () const
    {
        return get<T>(_value);
    }

    template <typename T>
    static T value (value_type const & value)
    {
        return get<T>(value);
    }

    template <typename T>
    static T value (property const & prop)
    {
        return get<T>(prop._value);
    }

    bool to_bool (bool * ok = nullptr) const
    {
        if (ok)
            *ok = true;

        if (is<bool_type>())
            return get<bool_type>(_value);

        if (is<int_type>())
            return get<int_type>(_value) != int_type{0} ? true : false;

        if (is<uint_type>())
            return get<uint_type>(_value) != uint_type{0} ? true : false;

        if (is<real_type>())
            return get<real_type>(_value) != real_type{0} ? true : false;

        if (ok)
            *ok = false;

        return false;
    }

    int_type to_int (bool * ok = nullptr) const
    {
        if (ok)
            *ok = true;

        if (is<int_type>())
            return get<int_type>(_value);

        if (is<bool_type>())
            return get<bool_type>(_value) ? int_type{1} : int_type{0};

        if (is<uint_type>())
            return static_cast<int_type>(get<uint_type>(_value));

        if (is<real_type>())
            return static_cast<int_type>(get<real_type>(_value));

        if (ok)
            *ok = false;

        return int_type{0};
    }

    uint_type to_uint (bool * ok = nullptr) const
    {
        if (ok)
            *ok = true;

        if (is<uint_type>())
            return get<uint_type>(_value);

        if (is<bool_type>())
            return get<bool_type>(_value) ? uint_type{1}: uint_type{0};

        if (is<int_type>())
            return static_cast<uint_type>(get<int_type>(_value));

        if (is<real_type>())
            return static_cast<uint_type>(get<real_type>(_value));

        if (ok)
            *ok = false;

        return uint_type{0};
    }

    real_type to_real (bool * ok = nullptr) const
    {
        if (ok)
            *ok = true;

        if (is<real_type>())
            return get<real_type>(_value);

        if (is<bool_type>())
            return get<bool_type>(_value) ? real_type{1}: real_type{0};

        if (is<int_type>())
            return static_cast<real_type>(get<int_type>(_value));

        if (is<uint_type>())
            return static_cast<real_type>(get<uint_type>(_value));

        if (ok)
            *ok = false;

        return real_type{0};
    }

    string_type to_string () const
    {
        if (is<string_type>())
            return get<string_type>(_value);

        if (is<bool_type>())
            return std::to_string(get<bool_type>(_value));

        if (is<int_type>())
            return std::to_string(get<int_type>(_value));

        if (is<uint_type>())
            return std::to_string(get<uint_type>(_value));

        if (is<real_type>())
            return std::to_string(get<real_type>(_value));

        return string_type{};
    }
};

class abstract_settings_plugin
{
public:
    using string_type = std::string;

public:
    emitter_mt<string_type const &> log_error;

public:
    virtual ~abstract_settings_plugin () {}

    virtual void set (property::key_type const & /*key*/
        , property::value_type const & /*value*/) = 0;

    virtual property get (property::key_type const & key
        , property::value_type const & default_value) = 0;

    void set (property::key_type const & key
        , char const * value)
    {
        set(key, property::string_type{value});
    }

    void set (char const * key, char const * value)
    {
        set(property::string_type{key}, property::string_type{value});
    }

    property get (property::key_type const & key
        , char const * default_value)
    {
        return get(key, property::string_type{default_value});
    }

    property get (char const * key, char const * default_value)
    {
        return get(property::string_type{key}, property::string_type{default_value});
    }
};

class null_settings_plugin: public abstract_settings_plugin
{
public:
    void set (property::key_type const & /*key*/
        , property::value_type const & /*value*/) override
    {}

    property get (property::key_type const & key
        , property::value_type const & default_value) override
    {
        return property{default_value};
    }
};

}} // namespace pfs::modulus
