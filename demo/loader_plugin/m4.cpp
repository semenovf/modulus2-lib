////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2024 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2024.07.12 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "api.hpp"
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"

using modulus2 = modulus::modulus2<modulus::iostream_logger, modulus::null_settings>;

class m4 : public modulus2::regular_module
{
public:
    bool on_start () override
    {
        log_debug("m4::on_start()");
        return false;
    }
};

extern "C" {

MODULUS2__MODULE_EXPORT modulus2::basic_module * __module_ctor__ (void)
{
    std::cout << "+Construct m4\n";
    auto m = new m4;
    return m;
}

MODULUS2__MODULE_EXPORT void  __module_dtor__ (modulus2::basic_module * m)
{
    std::cout << "~Destruct 'm4'\n";
    delete m;
}

} // extern "C"
