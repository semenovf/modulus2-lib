////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2025 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.05.20 Initial version (inherited from https://github.com/semenovf/pfs-modulus)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/modulus/modulus.hpp"
#include "pfs/modulus/iostream_logger.hpp"
#include "pfs/modulus/plugins/timer_quit.hpp"

using modulus_t = modulus::modulus<modulus::iostream_logger, modulus::null_settings>;

std::atomic_int __timer_counter {0};

class custom_module_lifetime_plugin: public modulus::module_lifetime_plugin
{
    std::string _prefix {"++ Lifetime plugin: "};

protected:
    void on_module_registered (std::string const & name) override
    {
        std::cout << _prefix << "module registered: " << name << std::endl;
    }

    void on_module_unregistered (std::string const & name) override
    {
        std::cout << _prefix << "module unregistered: " << name << std::endl;
    }
};

struct Data
{
    std::string name {"a"};
    std::string value {"b"};
};

class m1 : public modulus_t::regular_module
{
    int _counter = 0;

    modulus_t::emitter_type<> emitZeroArg;
    modulus_t::emitter_type<bool> emitOneArg;
    modulus_t::emitter_type<bool, char> emitTwoArgs;
    modulus_t::emitter_type<bool, char, short> emitThreeArgs;
    modulus_t::emitter_type<bool, char, short, int> emitFourArgs;
    modulus_t::emitter_type<bool, char, short, int, long> emitFiveArgs;
    modulus_t::emitter_type<bool, char, short, int, long, std::string> emitSixArgs;
    modulus_t::emitter_type<Data> emitData;

private:
    bool on_start () override
    {
        log_debug("on_start()");

        emitZeroArg();

        emitOneArg(true);

        emitTwoArgs(true, 'c');

        emitThreeArgs(true, 'c'
                , std::numeric_limits<short>::max());

        emitFourArgs(true, 'c'
                , std::numeric_limits<short>::max()
                , std::numeric_limits<int>::max());

        emitFiveArgs(true, 'c'
                , std::numeric_limits<short>::max()
                , std::numeric_limits<int>::max()
                , std::numeric_limits<int>::max());

        emitSixArgs(true, 'c'
                , std::numeric_limits<short>::max()
                , std::numeric_limits<int>::max()
                , std::numeric_limits<int>::max()
                , "Hello, World!");

        Data d;
        d.name = std::string{"Name"};
        d.value = std::string{"Value"};

        emitData(std::move(d));

        start_timer(std::chrono::milliseconds(100), [] {
            __timer_counter++;
        });

        return true;
    }

    bool on_finish () override
    {
        log_debug("on_finish()");
        return true;
    }

public:
    m1 () {}

    ~m1 ()
    {
        CHECK(_counter == 1);
    }

    void declare_emitters (modulus_t::module_context & ctx) override
    {
        ctx.declare_emitter(0, emitZeroArg);
        ctx.declare_emitter(1, emitOneArg);
        ctx.declare_emitter(2, emitTwoArgs);
        ctx.declare_emitter(3, emitThreeArgs);
        ctx.declare_emitter(4, emitFourArgs);
        ctx.declare_emitter(5, emitFiveArgs);
        ctx.declare_emitter(6, emitSixArgs);
        ctx.declare_emitter(7, emitData);
    }

    bool connect_detector (modulus_t::api_id_type id
        , modulus_t::module_context & ctx) override
    {
        switch (id) {
            case 4:
                return ctx.connect_detector(id, *this, & m1::onFourArgs);
        }

        return false;
    }

private:
    void onFourArgs (bool ok, char, short, int i)
    {
        _counter++;
        CHECK(ok);
        CHECK(i == std::numeric_limits<int>::max());
    }
};

class m2 : public modulus_t::regular_module
{
    int _counter = 0;

private:
    bool on_start () override
    {
        log_debug("on_start()");

        start_timer(std::chrono::milliseconds(200), [] {
            __timer_counter++;
        });

        return true;
    }

    bool on_finish () override
    {
        log_debug("on_finish()");
        return true;
    }

public:
    m2 (int) {}

    ~m2 ()
    {
        CHECK(_counter == 8);
    }

    bool connect_detector (modulus_t::api_id_type id
        , modulus_t::module_context & ctx) override
    {
        switch (id) {
            case 0:
                return ctx.connect_detector(id, *this, & m2::onZeroArg);
            case 1:
                return ctx.connect_detector(id, *this, & m2::onOneArg);
            case 2:
                return ctx.connect_detector(id, *this, & m2::onTwoArgs);
            case 3:
                return ctx.connect_detector(id, *this, & m2::onThreeArgs);
            case 4:
                return ctx.connect_detector(id, *this, & m2::onFourArgs);
            case 5:
                return ctx.connect_detector(id, *this, & m2::onFiveArgs);
            case 6:
                return ctx.connect_detector(id, *this, & m2::onSixArgs);
            case 7:
                return ctx.connect_detector(id, *this, & m2::onData);
        }

        return false;
    }

private:
    void onZeroArg ()
    {
        _counter++;
    }

    void onOneArg (bool ok)
    {
        _counter++;
        CHECK(ok);
    }

    void onTwoArgs (bool ok, char ch)
    {
        _counter++;
        CHECK(ok);
        CHECK(ch == 'c');
    }

    void onThreeArgs (bool ok, char, short i)
    {
        _counter++;
        CHECK(ok);
        CHECK(i == std::numeric_limits<short>::max());
    }

    void onFourArgs (bool ok, char, short, int i)
    {
        _counter++;
        CHECK(ok);
        CHECK(i == std::numeric_limits<int>::max());
    }

    void onFiveArgs (bool ok, char, short, int, long i)
    {
        _counter++;
        CHECK(ok);
        CHECK(i == std::numeric_limits<int>::max());
    }

    void onSixArgs (bool ok, char, short, int, long, std::string hello)
    {
        _counter++;
        CHECK(ok);
        CHECK(std::strcmp("Hello, World!", hello.c_str()) == 0);
    }

    void onData (Data const & d)
    {
        _counter++;
        CHECK(d.name == "Name");
        CHECK(d.value == "Value");
    }
};

class m3 : public modulus_t::runnable_module
{
private:
    bool on_start () override
    {
        log_debug("on_start()");

        start_timer(std::chrono::milliseconds(300), [] {
            __timer_counter++;
        });

        return true;
    }

    bool on_finish () override
    {
        log_debug("on_finish()");
        return true;
    }

public:
    m3 (int, std::string const &) {}

    modulus_t::exit_status run () override
    {
        while (! is_quit()) {
            if (call_all() == 0)
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        quit();

        return modulus_t::exit_status::success;
    }
};

class m4 : public modulus_t::guest_module
{
    modulus_t::emitter_type<bool> emitOneArg;
    modulus_t::emitter_type<bool, char> emitTwoArgs;

    int _counter = 0;

private:
    bool on_start () override
    {
        log_debug("on_start()");

        start_timer(std::chrono::milliseconds(400), [] {
            __timer_counter++;
        });

        return true;
    }

    bool on_finish () override
    {
        log_debug("on_finish()");
        return true;
    }

public:
    m4 () {}

    ~m4 ()
    {
        CHECK(_counter == 3);
    }

    void declare_emitters (modulus_t::module_context & ctx) override
    {
        ctx.declare_emitter(1, emitOneArg);
        ctx.declare_emitter(2, emitTwoArgs);
    }

    bool connect_detector (modulus_t::api_id_type id
        , modulus_t::module_context & ctx) override
    {
        switch (id) {
            case 1:
                return ctx.connect_detector(id, *this, & m4::onOneArg);
            case 2:
                return ctx.connect_detector(id, *this, & m4::onTwoArgs);
            case 7:
                return ctx.connect_detector(id, *this, & m4::onData);
        }

        return false;
    }

private:
    void onOneArg (bool ok)
    {
        _counter++;
        CHECK_MESSAGE(ok, "from guest_module: onOneArg(bool)");
    }

    void onTwoArgs (bool ok, char ch)
    {
        _counter++;
        CHECK(ok);
        CHECK_MESSAGE(ch == 'c', "from guest_module: onTwoArgs(true, 'c')");
    }

    void onData (Data const & d)
    {
        _counter++;
        CHECK_MESSAGE(d.name == "Name", "from guest_module: onData()");
        CHECK_MESSAGE(d.value == "Value", "from guest_module: onData()");
    }
};

class m5 : public modulus_t::guest_module
{
    modulus_t::emitter_type<> emitZeroArg;
    modulus_t::emitter_type<bool> emitOneArg;

    int _counter = 0;

private:
    bool on_start () override
    {
        log_debug("on_start()");

        start_timer(std::chrono::milliseconds(500), [] {
            __timer_counter++;
        });

        return true;
    }

    bool on_finish () override
    {
        log_debug("on_finish()");
        return true;
    }

public:
    m5 () {}

    ~m5 ()
    {
        CHECK(_counter == 2);
    }

    void declare_emitters (modulus_t::module_context & ctx) override
    {
        ctx.declare_emitter(0, emitZeroArg);
        ctx.declare_emitter(1, emitOneArg);
    }

    bool connect_detector (modulus_t::api_id_type id, modulus_t::module_context & ctx) override
    {
        switch (id) {
            case 0:
                return ctx.connect_detector(id, *this, & m5::onZeroArg);
            case 1:
                return ctx.connect_detector(id, *this, & m5::onOneArg);
        }

        return false;
    }

private:
    void onZeroArg ()
    {
        _counter++;
    }

    void onOneArg (bool ok)
    {
        _counter++;
        CHECK_MESSAGE(ok, "from guest_module: onOneArg(bool)");
    }
};

//
// For testing lambda as detector
//
class m6 : public modulus_t::guest_module
{
    int _counter = 0;

public:
    m6 () {}

    ~m6 ()
    {
        CHECK(_counter == 2);
    }

    bool connect_detector (modulus_t::api_id_type id, modulus_t::module_context & ctx) override
    {
        switch (id) {
            case 0:
                return ctx.connect_detector(id, *this, [this] () {
                    _counter++;
                });
            case 1:
                return ctx.connect_detector(id, *this, [this] (bool) {
                    _counter++;
                });
        }

        return false;
    }
};


class m7 : public modulus_t::runnable_module
{
    int _counter = 0;

private:
    bool on_start () override
    {
        log_debug("on_start()");

        start_timer(std::chrono::milliseconds(300), [] {
            __timer_counter++;
        });

        return true;
    }

    bool on_finish () override
    {
        log_debug("on_finish()");
        return true;
    }

    void flush () override
    {}

public:
    m7 () {}

    ~m7 ()
    {
        CHECK(_counter == 2);
    }

    bool connect_detector (modulus_t::api_id_type id, modulus_t::module_context & ctx) override
    {
        switch (id) {
            case 0:
                return ctx.connect_detector(id, *this, [this] () {
                    _counter++;
                });
            case 1:
                return ctx.connect_detector(id, *this, [this] (bool) {
                    _counter++;
                });
        }

        return false;
    }
};

TEST_CASE("Modulus basics") {
    using exit_status = modulus_t::exit_status;
    modulus_t::dispatcher d{std::make_shared<modulus::iostream_logger>(), modulus::null_settings{}};

    int timeout = 2; // seconds
    modulus::timer_quit_plugin timer_quit_plugin {timeout};

    custom_module_lifetime_plugin lifetime_plugin;
    d.attach_plugin(lifetime_plugin);

    CHECK(d.register_module<m1>(std::make_pair("m1", "")));
    CHECK(d.register_module<m2>(std::make_pair("m2", ""), 42));
    CHECK(d.register_module<m3>(std::make_pair("m3", ""), 43, "hello"));
    CHECK(d.register_module<m4>(std::make_pair("m4", "m3")));
    CHECK(d.register_module<m5>(std::make_pair("m5", "")));
    CHECK(d.register_module<m6>(std::make_pair("m6", "m3")));
    CHECK(d.register_module<m7>(std::make_pair("m7", "")));

    CHECK(d.count() == 7);

    d.attach_plugin(timer_quit_plugin);
    CHECK(d.exec() == exit_status::success);
    CHECK_EQ(__timer_counter, 6);
}
