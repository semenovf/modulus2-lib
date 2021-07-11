////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.07.11 Initial version.
////////////////////////////////////////////////////////////////////////////////
#include "pfs/modulus2.hpp"
#include "pfs/iostream_logger.hpp"

using modulus2 = pfs::modulus2<pfs::iostream_logger>;

#if defined(_MSC_VER)
__declspec(dllexport)
#endif
class m1 : public modulus2::regular_module
{
public:
    bool on_start (modulus2::properties_type const &) override
    {
        log_debug("m1::on_start()");
        return true;
    }

    bool on_finish () override
    {
        log_debug("m1::on_finish()");
        return true;
    }
};

extern "C" {

#if defined(_MSC_VER)
__declspec(dllexport)
#endif
modulus2::basic_module * __module_ctor__ (void)
{
    auto m = new m1;
    std::cout << "+Construct m1\n";
    return m;
}

#if defined(_MSC_VER)
__declspec(dllexport)
#endif
void  __module_dtor__ (modulus2::basic_module * m)
{
    std::cout << "~Destruct 'm1'\n";
    delete m;
}

} // extern "C"

