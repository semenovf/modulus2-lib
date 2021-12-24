////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.06.18 Initial version
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/emitter.hpp"

namespace modulus {

class quit_plugin
{
public:
    using emitter_type = pfs::emitter_mt<>;

public:
    emitter_type quit;

    quit_plugin () = default;
    ~quit_plugin () = default;

    quit_plugin (quit_plugin const &) = delete;
    quit_plugin & operator = (quit_plugin const &) = delete;
    quit_plugin (quit_plugin &&) = delete;
    quit_plugin & operator = (quit_plugin &&) = delete;
};

} // namespace modulus
