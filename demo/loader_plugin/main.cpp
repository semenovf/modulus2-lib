////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.07.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"
#include "pfs/modulus2/posix_quit_plugin.hpp"
#include "pfs/modulus2/dl_loader_plugin.hpp"

using modulus2_type = pfs::modulus::modulus2<pfs::modulus::iostream_logger>;
using namespace pfs::modulus;

int main ()
{
    using exit_status = modulus2_type::exit_status;

    iostream_logger logger;
    modulus2_type::dispatcher d{logger};
    posix_quit_plugin posix_quit_plugin;
    dl_loader_plugin<modulus2_type> dl_loader_plugin;

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
