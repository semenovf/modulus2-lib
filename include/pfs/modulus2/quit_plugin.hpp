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

namespace pfs {

class quit_plugin
{
public:
    using emitter_type = emitter_mt<>;

public:
    emitter_type quit;
};

} // namespace

