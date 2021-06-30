////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.06.30 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>

namespace pfs {

inline std::string const_prefix () { return std::string{"C"}; }
inline std::string ptr_prefix () { return std::string{"P"}; }
inline std::string ref_prefix () { return std::string{"R"}; }
inline std::string constptr_prefix () { return std::string{"CP"}; }
inline std::string constref_prefix () { return std::string{"CR"}; }

template <typename T>
struct mangling
{
    std::string operator () () const noexcept
    {
        return std::string{"?"};
    }
};

template <typename T>
struct mangling<T const>
{
    std::string operator () () const noexcept
    {
        return const_prefix() + mangling<T>{}();
    }
};

template <typename T>
struct mangling<T *>
{
    std::string operator () () const noexcept
    {
        return ptr_prefix() + mangling<T>{}();
    }
};

template <typename T>
struct mangling<T const *>
{
    std::string operator () () const noexcept
    {
        return constptr_prefix() + mangling<T>{}();
    }
};

template <typename T>
struct mangling<T &>
{
    std::string operator () () const noexcept
    {
        return ref_prefix() + mangling<T>{}() ;
    }
};

template <typename T>
struct mangling<T const &>
{
    std::string operator () () const noexcept
    {
        return constref_prefix() + mangling<T>{}() ;
    }
};

#define __PFS_MANGLING_PRIMITIVE_TYPES(t,c)        \
    template <>                                    \
    struct mangling<t>                             \
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
struct sequence_mangling;

template <typename T, typename ...Ts>
struct sequence_mangling<T, Ts...>
{
    std::string operator () () const noexcept
    {
        auto tail = sequence_mangling<Ts...>{}();
        return mangling<T>{}() + (tail.empty() ? "" : ";") + tail;
    }
};

template <>
struct sequence_mangling<>
{
    std::string operator () () const noexcept
    {
        return "";
    }
};

template <typename ...Ts>
inline std::string mangle ()
{
    return sequence_mangling<Ts...>{}();
}

} // namespace pfs
