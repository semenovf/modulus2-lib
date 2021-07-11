////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.07.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2.hpp"
#include "pfs/iostream_logger.hpp"
#include "pfs/modulus2/posix_quit_plugin.hpp"
#include "pfs/modulus2/dl_loader_plugin.hpp"

using modulus2 = pfs::modulus2<pfs::iostream_logger>;

int main ()
{
    using exit_status = modulus2::exit_status;

    pfs::iostream_logger logger;
    modulus2::dispatcher d{logger};
    pfs::posix_quit_plugin posix_quit_plugin;
    pfs::dl_loader_plugin<modulus2> dl_loader_plugin;

    d.attach_plugin(posix_quit_plugin);
    d.attach_plugin(dl_loader_plugin);

    std::list<std::string> search_paths;

#if defined(_MSC_VER)
    d.register_module_for_path(std::make_pair("m1", ""), "m1.dll", search_paths);
#else
    d.register_module_for_path(std::make_pair("m1", ""), "libm1.so", search_paths);
#endif

    d.register_module_for_name(std::make_pair("m2", ""), "m2", search_paths);

    return d.exec() == exit_status::success ? 0 : -1;
}
