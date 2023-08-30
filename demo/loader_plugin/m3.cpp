////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2023 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2023.08.30 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2/bundle.hpp"
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"

using modulus2 = modulus::modulus2<modulus::iostream_logger, modulus::null_settings>;

class m3 : public modulus2::regular_module
{
public:
    bool on_start () override
    {
        log_debug("m3::on_start()");
        return true;
    }

    bool on_finish () override
    {
        log_debug("m3::on_finish()");
        return true;
    }
};

extern "C" {

MODULUS2__MODULE_EXPORT modulus2::basic_module * __module_ctor_bundle__ (modulus::bundle const & args)
{
    auto m = new m3;
    std::cout << "+Construct m3 ("
        << "\n\tboolean=" << args.get("boolean", false)
        << "\n\tdouble=" << args.get<double>("double", 0)
        << ")\n";
    return m;
}

MODULUS2__MODULE_EXPORT void  __module_dtor__ (modulus2::basic_module * m)
{
    std::cout << "~Destruct 'm3'\n";
    delete m;
}

} // extern "C"

