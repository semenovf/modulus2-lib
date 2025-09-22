////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.06.18 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../namespace.hpp"
#include <pfs/emitter.hpp>

MODULUS__NAMESPACE_BEGIN

class quit_plugin
{
public:
    using emitter_type = pfs::emitter_mt<int>;

public:
    emitter_type quit;

    quit_plugin () = default;
    ~quit_plugin () = default;

    quit_plugin (quit_plugin const &) = delete;
    quit_plugin & operator = (quit_plugin const &) = delete;
    quit_plugin (quit_plugin &&) = delete;
    quit_plugin & operator = (quit_plugin &&) = delete;
};

MODULUS__NAMESPACE_END
