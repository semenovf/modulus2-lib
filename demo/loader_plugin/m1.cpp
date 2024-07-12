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
#include "api.hpp"
#include "pfs/modulus2/modulus2.hpp"
#include "pfs/modulus2/iostream_logger.hpp"

using modulus2 = modulus::modulus2<modulus::iostream_logger, modulus::null_settings>;

class m1 : public modulus2::runnable_module
{

private:
    modulus2::emitter_type<> emitMethod1;
    modulus2::emitter_type<int> emitMethod2;

public:
    void declare_emitters (modulus2::module_context & ctx) override
    {
        ctx.declare_emitter(API_METHOD1, emitMethod1); // => bbridge
        ctx.declare_emitter(API_METHOD2, emitMethod2); // => delivery
    }

    bool connect_detector (modulus2::api_id_type id, modulus2::module_context & ctx) override
    {
        return false;
    }

public:
    bool on_start () override
    {
        log_debug("m1::on_start()");
        return true;
    }

    bool on_finish () override
    {
        log_debug("m1::on_finish()");
        return true;
    }

    modulus2::exit_status run () override
    {
        quit();
        return modulus2::exit_status::success;
    }
};

extern "C" {

MODULUS2__MODULE_EXPORT modulus2::basic_module * __module_ctor__ (void)
{
    auto m = new m1;
    std::cout << "+Construct m1\n";
    return m;
}

MODULUS2__MODULE_EXPORT void  __module_dtor__ (modulus2::basic_module * m)
{
    std::cout << "~Destruct 'm1'\n";
    delete m;
}

} // extern "C"

