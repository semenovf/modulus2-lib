////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.07.11 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"

using modulus2 = pfs::modulus::modulus2<pfs::modulus::iostream_logger>;

#if defined(_MSC_VER)
__declspec(dllexport)
#endif
class m2 : public modulus2::regular_module
{
public:
    bool on_start (modulus2::properties_type const &) override
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

#if defined(_MSC_VER)
__declspec(dllexport)
#endif
modulus2::basic_module * __module_ctor__ (void)
{
    auto m = new m2;
    std::cout << "+Construct m2\n";
    return m;
}

#if defined(_MSC_VER)
__declspec(dllexport)
#endif
void  __module_dtor__ (modulus2::basic_module * m)
{
    std::cout << "~Destruct 'm2'\n";
    delete m;
}

} // extern "C"
