////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2022 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.07.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "loader.hpp"
#include "pfs/dynamic_library.hpp"
#include "pfs/i18n.hpp"
#include <memory>
#include <utility>

#if ANDROID
#   include <android/log.h>
#endif

namespace modulus {

namespace fs = pfs::filesystem;

template <typename ModulusType>
class dl_loader_plugin: public loader_plugin<ModulusType>
{
protected:
    using base_type = loader_plugin<ModulusType>;
    using basic_module_type = typename base_type::basic_module_type;
    using module_pointer = typename base_type::module_pointer;
    using module_deleter = typename base_type::module_deleter;
    using basic_module_deleter = typename base_type::basic_module_deleter;

    struct dynamic_module_deleter : public basic_module_deleter
    {
        std::shared_ptr<pfs::dynamic_library> dlptr;
        std::function<void(basic_module_type*)> dtor;

        dynamic_module_deleter (std::shared_ptr<pfs::dynamic_library> p, std::function<void(basic_module_type*)> d)
            : dlptr(p)
            , dtor(d)
        {}

        void operator () (basic_module_type * m) const override
        {
            if (dtor)
                dtor(m);
        }
    };

public:
    std::pair<module_pointer, std::string> load_module_for_path (fs::path const & path
        , std::list<fs::path> const & search_dirs) override
    {
        return module_for_path(path, search_dirs.begin(), search_dirs.end());
    }

    std::pair<module_pointer, std::string> load_module_for_name (std::string const & basename
        , std::list<fs::path> const & search_dirs) override
    {

        return module_for_name(basename, search_dirs.begin(), search_dirs.end());
    }

protected:
    template <typename ForwardPathIt>
    std::pair<module_pointer, std::string> module_for_path (fs::path const & path
        , ForwardPathIt first, ForwardPathIt last)
    {
        static char const * module_ctor_name = "__module_ctor__";
        static char const * module_dtor_name = "__module_dtor__";

        fs::path orig_path(path);
        fs::path dylib_path(path);

        if (orig_path.is_relative()) {
            if (first == last) {
                dylib_path = fs::current_path() / orig_path;
            } else {
                while (first != last) {
                    if (fs::exists(*first / orig_path)) {
                        dylib_path = *first / orig_path;
                        break;
                    }
                    ++first;
                }
            }
        }

        if (!fs::exists(dylib_path)) {
            // This is a critical section, so log output must not depends on logger
#if ANDROID
            __android_log_print(ANDROID_LOG_ERROR, "modulus"
                , "module not found: %s\n", dylib_path.c_str());
#else
            fmt::print(stderr, "module not found: {}\n", fs::utf8_encode(dylib_path));
#endif

            return std::make_pair(module_pointer{nullptr, module_deleter{}}
                , std::string{});
        } else {
#if ANDROID
            __android_log_print(ANDROID_LOG_DEBUG, "modulus"
                , "module found: %s\n", dylib_path.c_str());
#endif
        }

        std::shared_ptr<pfs::dynamic_library> dylib_ptr;

        try {
            dylib_ptr = std::make_shared<pfs::dynamic_library>(dylib_path);
        } catch (pfs::error ex) {
            // This is a critical section, so log output must not depends on logger
#if ANDROID
            __android_log_print(ANDROID_LOG_ERROR, "modulus"
                , "%s\n", ex.what().c_str());
#else
            fmt::print(stderr, "{}\n", ex.what());
#endif
            return std::make_pair(module_pointer{nullptr, module_deleter{}}, std::string{});
        }

        std::error_code ec;
        auto module_ctor = dylib_ptr->resolve<basic_module_type*(void)>(module_ctor_name, ec);

        if (ec) {
            this->failure(tr::f_("{}: failed to resolve constructor `{}' for module: {}"
                , fs::utf8_encode(dylib_path)
                , std::string(module_ctor_name)
                , ec.message()));

            return std::make_pair(module_pointer{nullptr, module_deleter{}}, std::string{});
        }

        auto module_dtor = dylib_ptr->resolve<void(basic_module_type*)>(module_dtor_name, ec);

        if (ec) {
            this->failure(tr::f_("{}: failed to resolve destructor `{}' for module: {}"
                , fs::utf8_encode(dylib_path)
                , std::string(module_dtor_name)
                , ec.message()));

            return std::make_pair(module_pointer{nullptr, module_deleter{}}, std::string{});
        }

        decltype(& *module_pointer{nullptr, module_deleter{}}) ptr = module_ctor();

        if (!ptr)
            std::make_pair(module_pointer{nullptr, module_deleter{}}, std::string{});

        return std::make_pair(module_pointer{ptr
            , module_deleter{new dynamic_module_deleter{dylib_ptr, module_dtor}}}
            , fs::utf8_encode(dylib_path));
    }

    template <typename ForwardPathIt>
    std::pair<module_pointer, std::string> module_for_name (std::string const & basename
        , ForwardPathIt first, ForwardPathIt last)
    {
        auto modpath = pfs::dynamic_library::build_filename(basename);
        return module_for_path<ForwardPathIt>(modpath, first, last);
    }
};

} // namespace modulus
