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
#include "api.hpp"
#include "pfs/modulus/modulus.hpp"
#include "pfs/modulus/iostream_logger.hpp"

using modulus_t = modulus::modulus<modulus::iostream_logger, modulus::null_settings>;

class m1 : public modulus_t::runnable_module
{

private:
    modulus_t::emitter_type<> emitMethod1;
    modulus_t::emitter_type<int> emitMethod2;

public:
    void declare_emitters (modulus_t::module_context & ctx) override
    {
        ctx.declare_emitter(API_METHOD1, emitMethod1); // => bbridge
        ctx.declare_emitter(API_METHOD2, emitMethod2); // => delivery
    }

    bool connect_detector (modulus_t::api_id_type id, modulus_t::module_context & ctx) override
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

    modulus_t::exit_status run () override
    {
        quit();
        return modulus_t::exit_status::success;
    }
};

extern "C" {

MODULUS__MODULE_EXPORT modulus_t::basic_module * __module_ctor__ (void)
{
    auto m = new m1;
    std::cout << "+Construct m1\n";
    return m;
}

MODULUS__MODULE_EXPORT void  __module_dtor__ (modulus_t::basic_module * m)
{
    std::cout << "~Destruct 'm1'\n";
    delete m;
}

} // extern "C"

