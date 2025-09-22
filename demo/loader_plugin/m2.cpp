////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2025 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.07.11 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus/modulus.hpp"
#include "pfs/modulus/iostream_logger.hpp"

using modulus_t = modulus::modulus<modulus::iostream_logger, modulus::null_settings>;

class m2 : public modulus_t::regular_module
{
public:
    bool on_start () override
    {
        log_debug("m2::on_start()");
        return true;
    }

    bool on_finish () override
    {
        log_debug("m2::on_finish()");
        return true;
    }
};

extern "C" {

MODULUS__MODULE_EXPORT modulus_t::basic_module * __module_ctor__ (void)
{
    auto m = new m2;
    std::cout << "+Construct m2\n";
    return m;
}

MODULUS__MODULE_EXPORT void  __module_dtor__ (modulus_t::basic_module * m)
{
    std::cout << "~Destruct 'm2'\n";
    delete m;
}

} // extern "C"
