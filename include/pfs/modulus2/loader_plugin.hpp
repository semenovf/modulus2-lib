////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.07.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/emitter.hpp"
#include <string>

namespace pfs {

template <typename ModulusType>
class loader_plugin
{
public:
    using string_type = std::string;
    using basic_module_type = typename ModulusType::basic_module;
    using basic_module_deleter = typename ModulusType::basic_module_deleter;
    using module_pointer = typename ModulusType::module_pointer;
    using module_deleter = typename ModulusType::module_deleter;

public:
    emitter_mt<string_type const &> log_error;
    virtual module_pointer load_module_for_path (string_type const & path
        , std::list<string_type> const & search_dirs) = 0;
    virtual module_pointer load_module_for_name (string_type const & name
        , std::list<string_type> const & search_dirs) = 0;

public:
    loader_plugin () = default;
    ~loader_plugin () = default;

    loader_plugin (loader_plugin const &) = delete;
    loader_plugin & operator = (loader_plugin const &) = delete;
    loader_plugin (loader_plugin &&) = delete;
    loader_plugin & operator = (loader_plugin &&) = delete;
};

} // namespace pfs


