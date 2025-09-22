////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2025 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2023.08.30 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus/bundle.hpp"
#include "pfs/modulus/modulus.hpp"
#include "pfs/modulus/iostream_logger.hpp"

using modulus_t = modulus::modulus<modulus::iostream_logger, modulus::null_settings>;

class m3 : public modulus_t::regular_module
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

MODULUS2__MODULE_EXPORT modulus_t::basic_module * __module_ctor_bundle__ (modulus::bundle const & args)
{
    auto m = new m3;
    std::cout << "+Construct m3 ("
        << "\n\tboolean=" << args.get("boolean", false)
        << "\n\tdouble=" << args.get<double>("double", 0)
        << ")\n";
    return m;
}

MODULUS2__MODULE_EXPORT void  __module_dtor__ (modulus_t::basic_module * m)
{
    std::cout << "~Destruct 'm3'\n";
    delete m;
}

} // extern "C"

