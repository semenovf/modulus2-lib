////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.07.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"
#include "pfs/modulus2/plugins/platform_quit.hpp"
#include "pfs/modulus2/plugins/dl_loader.hpp"

using modulus2_type = modulus::modulus2<modulus::iostream_logger, modulus::null_settings>;

int main ()
{
    using exit_status = modulus2_type::exit_status;

    modulus::platform_quit_plugin quit_plugin;
    modulus::dl_loader_plugin<modulus2_type> dl_loader_plugin;

    modulus2_type::dispatcher d{modulus::iostream_logger{}, modulus::null_settings{}};

    d.attach_plugin(quit_plugin);
    d.attach_plugin(dl_loader_plugin);

    std::list<pfs::filesystem::path> search_paths;

#if defined(_MSC_VER)
    d.register_module_for_path(std::make_pair("m1", ""), "m1.dll", search_paths);
#else
    d.register_module_for_path(std::make_pair("m1", ""), "libm1.so", search_paths);
#endif

    d.register_module_for_name(std::make_pair("m2", ""), "m2", search_paths);

    modulus::bundle args;
    args.put("boolean", true);
    args.put("double", double{3.14});

    d.register_module_for_name(std::make_pair("m3", ""), "m3", search_paths, args);

    return d.exec() == exit_status::success ? 0 : -1;
}
