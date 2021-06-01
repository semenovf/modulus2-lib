////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2021.05.20 Initial version (inherited from https://github.com/semenovf/pfs-modulus)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <iostream>

namespace pfs {

class iostream_logger
{
public:
    void trace (std::string const & msg)
    {
        std::cout << "~~ " << msg.c_str() << "\n";
    }

    void debug (std::string const & msg)
    {
        std::cout << "-- " << msg.c_str() << "\n";
    }

    void info (std::string const & msg)
    {
        std::cout << msg.c_str() << "\n";
    }

    void warn (std::string const & msg)
    {
        std::cerr << "WARN: " << msg.c_str() << "\n";
    }

    void error (std::string const & msg)
    {
        std::cerr << "ERROR: " << msg.c_str() << "\n";
    }
};

class wiostream_logger
{
public:
    void info (std::wstring const & msg)
    {
        std::wcout << msg.c_str() << "\n";
    }

    void debug (std::wstring const & msg)
    {
        std::wcout << L"-- " << msg.c_str() << L"\n";
    }

    void warn (std::wstring const & msg)
    {
        std::wcerr << L"WARN: " << msg.c_str() << L"\n";
    }

    void error (std::wstring const & msg)
    {
        std::wcerr << L"ERROR: " << msg.c_str() << L"\n";
    }
};

} // namespace pfs