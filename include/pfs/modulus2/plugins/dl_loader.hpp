////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.07.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "loader.hpp"
#include "pfs/dynamic_library.hpp"
#include "pfs/fmt.hpp"
#include <memory>

#if ANDROID
#   include <android/log.h>
#endif

namespace modulus {

namespace fs = pfs::filesystem;

template <typename ModulusType>
class dl_loader_plugin: public loader_plugin<ModulusType>
{
    using base_type = loader_plugin<ModulusType>;
    using basic_module_type = typename base_type::basic_module_type;
    using module_pointer = typename base_type::module_pointer;
    using module_deleter = typename base_type::module_deleter;
    using basic_module_deleter = typename base_type::basic_module_deleter;
    using module_ctor_t = basic_module_type * (*)(void);
    using module_dtor_t = void (*)(basic_module_type *);

    struct dynamic_module_deleter : public basic_module_deleter
    {
        std::shared_ptr<pfs::dynamic_library> dlptr;
        module_dtor_t dtor {nullptr};

        dynamic_module_deleter (std::shared_ptr<pfs::dynamic_library> p, module_dtor_t d)
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
    module_pointer load_module_for_path (std::string const & path
        , std::list<std::string> const & search_dirs) override
    {
        return module_for_path(path, search_dirs.begin(), search_dirs.end());
    }

    module_pointer load_module_for_name (std::string const & basename
        , std::list<std::string> const & search_dirs) override
    {

        return module_for_name(basename, search_dirs.begin(), search_dirs.end());
    }

private:

    template <typename ForwardPathIt>
    module_pointer module_for_path (std::string const & path
        , ForwardPathIt first, ForwardPathIt last)
    {
        static char const * module_ctor_name = "__module_ctor__";
        static char const * module_dtor_name = "__module_dtor__";

        //auto dylib_ptr = std::make_shared<pfs::dynamic_library>();
        //std::error_code ec;

        fs::path orig_path(path);
        fs::path dylib_path(path);

        if (orig_path.is_relative()) {
            if (first == last) {
                dylib_path = fs::path(".") / orig_path;
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
#if ANDROID
            __android_log_print(ANDROID_LOG_ERROR, "modulus"
                , "module not found: %s\n", dylib_path.c_str());
#else
            fmt::print(stderr, "module not found: {}\n", fs::utf8_encode(dylib_path));
#endif

            return module_pointer{nullptr, module_deleter{}};
        }

        pfs::error err;
        auto dylib_ptr = std::make_shared<pfs::dynamic_library>(dylib_path, & err);

        if (!*dylib_ptr) {
            // This is a critical section, so log output must not depends on logger
#if ANDROID
            __android_log_print(ANDROID_LOG_ERROR, "modulus"
                , "open module failed: %s: %s\n"
                , dylib_path.c_str()
                , err.what().c_str());
#else
            fmt::print(stderr, "open module failed: {}: {}\n"
                , fs::utf8_encode(dylib_path)
                , err.what());
#endif
            return module_pointer{nullptr, module_deleter{}};
        }

        auto module_ctor = dylib_ptr->resolve<module_ctor_t>(module_ctor_name, & err);

        if (err) {
            this->failure(fmt::format("{}: failed to resolve constructor `{}' for module: {}"
                , fs::utf8_encode(dylib_path)
                , std::string(module_ctor_name)
                , err.what()));

            return module_pointer{nullptr, module_deleter{}};
        }

        //pfs::dynamic_library::symbol_type dtor = dylib_ptr->resolve(module_dtor_name, ec);
        auto module_dtor = dylib_ptr->resolve<module_dtor_t>(module_dtor_name, & err);

        if (err) {
            this->failure(fmt::format("{}: failed to resolve destructor `{}' for module: {}"
                , fs::utf8_encode(dylib_path)
                , std::string(module_dtor_name)
                , err.what()));

            return module_pointer{nullptr, module_deleter{}};
        }

        decltype(& *module_pointer{nullptr, module_deleter{}}) ptr = (*module_ctor)();

        if (!ptr)
            return module_pointer{nullptr, module_deleter{}};

        return module_pointer{ptr
            , module_deleter{new dynamic_module_deleter{dylib_ptr, *module_dtor}}};
    }

    template <typename ForwardPathIt>
    module_pointer module_for_name (std::string const & basename
        , ForwardPathIt first, ForwardPathIt last)
    {
        auto modpath = pfs::dynamic_library::build_filename(basename);
        return module_for_path<ForwardPathIt>(modpath, first, last);
    }
};

} // namespace modulus
