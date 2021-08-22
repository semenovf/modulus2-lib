////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.08.22 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
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

    bool to_bool (bool * ok = nullptr) const
    {
        if (ok)
            *ok = true;

        if (holds_alternative<bool_type>(_value))
            return get<bool_type>(_value);

        if (holds_alternative<int_type>(_value))
            return get<int_type>(_value) != int_type{0} ? true : false;

        if (holds_alternative<uint_type>(_value))
            return get<uint_type>(_value) != uint_type{0} ? true : false;

        if (holds_alternative<real_type>(_value))
            return get<real_type>(_value) != real_type{0} ? true : false;

        if (ok)
            *ok = false;

        return false;
    }

    int_type to_int (bool * ok = nullptr) const
    {
        if (ok)
            *ok = true;

        if (holds_alternative<int_type>(_value))
            return get<int_type>(_value);

        if (holds_alternative<bool_type>(_value))
            return get<bool_type>(_value) ? int_type{1} : int_type{0};

        if (holds_alternative<uint_type>(_value))
            return static_cast<int_type>(get<uint_type>(_value));

        if (holds_alternative<real_type>(_value))
            return static_cast<int_type>(get<real_type>(_value));

        if (ok)
            *ok = false;

        return int_type{0};
    }

    uint_type to_uint (bool * ok = nullptr) const
    {
        if (ok)
            *ok = true;

        if (holds_alternative<uint_type>(_value))
            return get<uint_type>(_value);

        if (holds_alternative<bool_type>(_value))
            return get<bool_type>(_value) ? uint_type{1}: uint_type{0};

        if (holds_alternative<int_type>(_value))
            return static_cast<uint_type>(get<int_type>(_value));

        if (holds_alternative<real_type>(_value))
            return static_cast<uint_type>(get<real_type>(_value));

        if (ok)
            *ok = false;

        return uint_type{0};
    }

    real_type to_real (bool * ok = nullptr) const
    {
        if (ok)
            *ok = true;

        if (holds_alternative<real_type>(_value))
            return get<real_type>(_value);

        if (holds_alternative<bool_type>(_value))
            return get<bool_type>(_value) ? real_type{1}: real_type{0};

        if (holds_alternative<int_type>(_value))
            return static_cast<real_type>(get<int_type>(_value));

        if (holds_alternative<uint_type>(_value))
            return static_cast<real_type>(get<uint_type>(_value));

        if (ok)
            *ok = false;

        return real_type{0};
    }

    string_type to_string () const
    {
        if (holds_alternative<string_type>(_value))
            return get<string_type>(_value);

        if (holds_alternative<bool_type>(_value))
            return std::to_string(get<bool_type>(_value));

        if (holds_alternative<int_type>(_value))
            return std::to_string(get<int_type>(_value));

        if (holds_alternative<uint_type>(_value))
            return std::to_string(get<uint_type>(_value));

        if (holds_alternative<real_type>(_value))
            return std::to_string(get<real_type>(_value));

        return string_type{};
    }
};

class abstract_settings_plugin
{
public:
    virtual void set (property::key_type const & /*key*/
        , property::value_type const & /*value*/)
    {}

    virtual void set (property::key_type && /*key*/
        , property::value_type && /*value*/)
    {}

    virtual property get (property::key_type const & key
        , property::value_type const & default_value)
    {
        return property{default_value};
    }

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
{};

}} // namespace pfs::modulus
