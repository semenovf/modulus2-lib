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
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/modulus2/mangling.hpp"

using pfs::mangle;

template <typename T>
void check_primitives_mangling (char c)
{
    std::string basic(std::size_t{1}, c);

    CHECK_EQ(mangle<T>()        , basic);
    CHECK_EQ(mangle<const T>()  , pfs::const_prefix()    + basic);
    CHECK_EQ(mangle<T const>()  , pfs::const_prefix()    + basic);
    CHECK_EQ(mangle<T *>()      , pfs::ptr_prefix()      + basic);
    CHECK_EQ(mangle<T const *>(), pfs::constptr_prefix() + basic);
    CHECK_EQ(mangle<T &>()      , pfs::ref_prefix()      + basic);
    CHECK_EQ(mangle<T const &>(), pfs::constref_prefix() + basic);
}

TEST_CASE("Mangling primitive types") {
    check_primitives_mangling<bool>              ('b');
    check_primitives_mangling<char>              ('c');
    check_primitives_mangling<signed char>       ('a');
    check_primitives_mangling<unsigned char>     ('h');
    check_primitives_mangling<short>             ('s');
    check_primitives_mangling<unsigned short>    ('t');
    check_primitives_mangling<int>               ('i');
    check_primitives_mangling<unsigned int>      ('j');
    check_primitives_mangling<long>              ('l');
    check_primitives_mangling<unsigned long>     ('m');
    check_primitives_mangling<long long>         ('x');
    check_primitives_mangling<unsigned long long>('y');
}

TEST_CASE("Mangling collection of primitive types") {
    CHECK(mangle<bool>() == "b");
    CHECK_EQ(mangle<bool, char>(), "b;c");
    CHECK_EQ(mangle<bool, char, int>(), "b;c;i");
    CHECK_EQ(mangle<bool, char *, int &>(), "b;Pc;Ri");
    CHECK_EQ(mangle<bool const, char const *, int const &>(), "Cb;CPc;CRi");
}

struct X {};

TEST_CASE("Mangling mixed collection of types") {
    CHECK(mangle<X>() == "?");
    CHECK_EQ(mangle<bool, X>(), "b;?");
    CHECK_EQ(mangle<X, X const, X const *, X const &>(), "?;C?;CP?;CR?");
}

TEST_CASE("Match mangled signatures") {
    using namespace pfs;

    CHECK_EQ(match_mangled_signatures(
          mangle<>()
        , ""), result_of_matching::equal);

    CHECK_EQ(match_mangled_signatures(
          mangle<int, int const, int const *, int const &>()
        , "i;Ci;CPi;CRi"), result_of_matching::equal);

    CHECK_EQ(match_mangled_signatures(
          mangle<int, int const, int const *>()
        , "i;Ci;CPi;CRi"), result_of_matching::different_amount);

    CHECK_EQ(match_mangled_signatures(
          mangle<int, int const, int const *, int>()
        , "i;Ci;CPi;CRi"), result_of_matching::incompatible_types);

    CHECK_EQ(match_mangled_signatures(
          mangle<int, int const, int const *, X const &>()
        , "i;Ci;CPi;CR?"), result_of_matching::fuzzy_equality);
}
