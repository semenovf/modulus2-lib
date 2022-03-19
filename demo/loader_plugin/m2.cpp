////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.07.11 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"

using modulus2 = modulus::modulus2<modulus::iostream_logger>;

class m2 : public modulus2::regular_module
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

MODULUS2__MODULE_EXPORT modulus2::basic_module * __module_ctor__ (void)
{
    auto m = new m2;
    std::cout << "+Construct m2\n";
    return m;
}

MODULUS2__MODULE_EXPORT void  __module_dtor__ (modulus2::basic_module * m)
{
    std::cout << "~Destruct 'm2'\n";
    delete m;
}

} // extern "C"
