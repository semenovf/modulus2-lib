////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.07.10 Initial version.
//      2023.08.30 Added bundle class for use while module construction.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../namespace.hpp"
#include "../bundle.hpp"
#include <pfs/emitter.hpp>
#include <pfs/filesystem.hpp>
#include <string>

MODULUS__NAMESPACE_BEGIN

namespace fs = pfs::filesystem;

template <typename ModulusType>
class loader_plugin
{
public:
    using basic_module_type = typename ModulusType::basic_module;
    using basic_module_deleter = typename ModulusType::basic_module_deleter;
    using module_pointer = typename ModulusType::module_pointer;
    using module_deleter = typename ModulusType::module_deleter;

public:
    pfs::emitter_mt<std::string const &> failure;

public:
    virtual std::pair<module_pointer, std::string> load_module_for_path (fs::path const & path
        , std::list<fs::path> const & search_dirs, bundle const & args = bundle{}) = 0;
    virtual std::pair<module_pointer, std::string> load_module_for_name (std::string const & name
        , std::list<fs::path> const & search_dirs, bundle const & args = bundle{}) = 0;

public:
    loader_plugin () = default;
    ~loader_plugin () = default;

    loader_plugin (loader_plugin const &) = delete;
    loader_plugin & operator = (loader_plugin const &) = delete;
    loader_plugin (loader_plugin &&) = delete;
    loader_plugin & operator = (loader_plugin &&) = delete;
};

MODULUS__NAMESPACE_END
