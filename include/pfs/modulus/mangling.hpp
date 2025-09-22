////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.06.30 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include <pfs/split.hpp>
#include <string>
#include <vector>

MODULUS__NAMESPACE_BEGIN

inline std::string const_prefix () { return std::string{"C"}; }
inline std::string ptr_prefix () { return std::string{"P"}; }
inline std::string ref_prefix () { return std::string{"R"}; }
inline std::string constptr_prefix () { return std::string{"CP"}; }
inline std::string constref_prefix () { return std::string{"CR"}; }

inline std::string::value_type unregistered_type_char () noexcept
{
    return '?';
}

template <typename T>
struct mangler
{
    std::string operator () () const noexcept
    {
        return std::string(1, unregistered_type_char());
    }
};

template <typename T>
struct mangler<T const>
{
    std::string operator () () const noexcept
    {
        return const_prefix() + mangler<T>{}();
    }
};

template <typename T>
struct mangler<T *>
{
    std::string operator () () const noexcept
    {
        return ptr_prefix() + mangler<T>{}();
    }
};

template <typename T>
struct mangler<T const *>
{
    std::string operator () () const noexcept
    {
        return constptr_prefix() + mangler<T>{}();
    }
};

template <typename T>
struct mangler<T &>
{
    std::string operator () () const noexcept
    {
        return ref_prefix() + mangler<T>{}() ;
    }
};

template <typename T>
struct mangler<T const &>
{
    std::string operator () () const noexcept
    {
        return constref_prefix() + mangler<T>{}() ;
    }
};

#define __PFS_MANGLING_PRIMITIVE_TYPES(t,c)        \
    template <>                                    \
    struct mangler<t>                             \
    {                                              \
        std::string operator () () const noexcept  \
        {                                          \
            return std::string(std::size_t{1}, c); \
        }                                          \
    };

__PFS_MANGLING_PRIMITIVE_TYPES(bool              , 'b')
__PFS_MANGLING_PRIMITIVE_TYPES(char              , 'c')
__PFS_MANGLING_PRIMITIVE_TYPES(signed char       , 'a')
__PFS_MANGLING_PRIMITIVE_TYPES(unsigned char     , 'h')
__PFS_MANGLING_PRIMITIVE_TYPES(short             , 's')
__PFS_MANGLING_PRIMITIVE_TYPES(unsigned short    , 't')
__PFS_MANGLING_PRIMITIVE_TYPES(int               , 'i')
__PFS_MANGLING_PRIMITIVE_TYPES(unsigned int      , 'j')
__PFS_MANGLING_PRIMITIVE_TYPES(long              , 'l')
__PFS_MANGLING_PRIMITIVE_TYPES(unsigned long     , 'm')
__PFS_MANGLING_PRIMITIVE_TYPES(long long         , 'x')
__PFS_MANGLING_PRIMITIVE_TYPES(unsigned long long, 'y')

template <typename ...Ts>
struct signature_mangler;

template <typename T, typename ...Ts>
struct signature_mangler<T, Ts...>
{
    std::string operator () () const noexcept
    {
        auto tail = signature_mangler<Ts...>{}();
        return mangler<T>{}() + (tail.empty() ? "" : ";") + tail;
    }
};

template <>
struct signature_mangler<>
{
    std::string operator () () const noexcept
    {
        return "";
    }
};

template <typename ...Ts>
inline std::string mangle ()
{
    return signature_mangler<Ts...>{}();
}

enum class result_of_matching
{
      equal = 0
    , different_amount    // different amount of encoded types (critical)
    , incompatible_types  // incompatible types in same positions (critical)
    , fuzzy_equality      // if basic types are unregistered (warning)
};

inline result_of_matching match_mangled_signatures (
      std::string const & s1
    , std::string const & s2)
{
    using string_type = std::string;
    using vector_type = std::vector<string_type>;

    vector_type v1;
    vector_type v2;
    string_type separator{";"};

    split(std::back_inserter(v1)
        , s1.begin(), s1.end()
        , separator.begin(), separator.end()
        , pfs::keep_empty::yes);

    split(std::back_inserter(v2)
        , s2.begin(), s2.end()
        , separator.begin(), separator.end()
        , pfs::keep_empty::yes);

    if (v1.size() != v2.size())
        return result_of_matching::different_amount;

    result_of_matching result = result_of_matching::equal;

    for (vector_type::size_type i = 0, icount = v1.size()
            ; i < icount && result == result_of_matching::equal
            ; i++) {

        if (v1[i].size() != v2[i].size())
            return result_of_matching::incompatible_types;

        if (v1[i].empty())
            continue;

        auto basic_type1 = v1[i].back();
        auto basic_type2 = v2[i].back();

        if (basic_type1 == basic_type2 && basic_type1 == unregistered_type_char())
            result = result_of_matching::fuzzy_equality;

        for (string_type::size_type j = 0, jcount = v1[i].size(); j < jcount; j++) {
            if (v1[i][j] != v2[i][j]) {
                result = result_of_matching::incompatible_types;
                break;
            }
        }
    }

    return result;
}

MODULUS__NAMESPACE_END
