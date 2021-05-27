////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2021.05.20 Initial version (inherited from https://github.com/semenovf/pfs-modulus)
////////////////////////////////////////////////////////////////////////////////
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "pfs/modulus2.hpp"
#include "pfs/iostream_logger.hpp"

using modulus2 = pfs::modulus2<pfs::iostream_logger>;

struct Data
{
    std::string name {"a"};
    std::string value {"b"};
};

class m1 : public modulus2::regular_module
{
    modulus2::emitter_type<> emitZeroArg;
    modulus2::emitter_type<bool> emitOneArg;
    modulus2::emitter_type<bool, char> emitTwoArgs;
    modulus2::emitter_type<bool, char, short> emitThreeArgs;
    modulus2::emitter_type<bool, char, short, int> emitFourArgs;
    modulus2::emitter_type<bool, char, short, int, long> emitFiveArgs;
    modulus2::emitter_type<bool, char, short, int, long, std::string> emitSixArgs;
    modulus2::emitter_type<Data> emitData;

public:
    m1 () {}

    virtual void declare_emitters (modulus2::module_context & ctx) override
    {
        std::cout << "Declare emitters for module: " << this->name() << "\n";
        ctx.declare_emitter(0, emitZeroArg);
        ctx.declare_emitter(1, emitOneArg);
        ctx.declare_emitter(2, emitTwoArgs);
        ctx.declare_emitter(3, emitThreeArgs);
        ctx.declare_emitter(4, emitFourArgs);
        ctx.declare_emitter(5, emitFiveArgs);
        ctx.declare_emitter(6, emitSixArgs);
        ctx.declare_emitter(7, emitData);
    }
};

class m2 : public modulus2::regular_module
{
    int _counter = 0;

public:
    m2 (int) {}

    virtual void connect_detector (modulus2::api_id_type id
        , modulus2::dispatcher & d) override
    {
        std::cout << "Connect m2 detector for id: " << id << "\n";

        switch (id) {
            case 0:
                d.connect_detector(id, *this, & m2::onZeroArg);
                break;
            case 1:
                d.connect_detector(id, *this, & m2::onOneArg);
                break;
            case 2:
                d.connect_detector(id, *this, & m2::onTwoArgs);
                break;
            case 3:
                d.connect_detector(id, *this, & m2::onThreeArgs);
                break;
            case 4:
                d.connect_detector(id, *this, & m2::onFourArgs);
                break;
            case 5:
                d.connect_detector(id, *this, & m2::onFiveArgs);
                break;
            case 6:
                d.connect_detector(id, *this, & m2::onSixArgs);
                break;
            case 7:
                d.connect_detector(id, *this, & m2::onData);
                break;
        }
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

class m3 : public modulus2::regular_module
{
public:
    m3 (int, std::string const &) {}
};

class m4 : public modulus2::regular_module
{
public:
    m4 () {}
};

TEST_CASE("Modulus2 basics") {

    pfs::iostream_logger logger;
    modulus2::dispatcher d{logger};

    //d.register_api(1, std::function<void()>);

    CHECK(d.register_module<m1>(std::make_pair("m1", "")));
    CHECK(d.register_module<m2>(std::make_pair("m2", ""), 42));
    CHECK(d.register_module<m3>(std::make_pair("m3", ""), 43, "hello"));
    CHECK(d.register_module<m4>(std::make_pair("m4", "m3")));

//     CHECK(d.count() == 4);
//     CHECK(d.exec() == 0);
}
