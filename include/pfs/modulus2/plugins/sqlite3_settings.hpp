////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2022.12.22 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "debby_settings.hpp"
#include "pfs/debby/backend/sqlite3/database.hpp"

namespace modulus {

using sqlite3_settings_plugin = debby_settings_plugin<debby::backend::sqlite3::database>;

} // namespace modulus

