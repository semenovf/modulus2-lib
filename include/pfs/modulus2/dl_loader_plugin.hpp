////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.07.10 Initial version.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "loader_plugin.hpp"
#include "pfs/dynamic_library.hpp"
#include "pfs/fmt.hpp"
#include <memory>

#if ANDROID
#   include <android/log.h>
#endif

namespace pfs {

#if defined(PFS_NO_STD_FILESYSTEM)
    namespace fs = pfs::filesystem;
#else
    namespace fs = std::filesystem;
#endif

template <typename ModulusType>
class dl_loader_plugin: public loader_plugin<ModulusType>
{
    using base_type = loader_plugin<ModulusType>;
    using string_type = typename base_type::string_type;
    using module_pointer = typename base_type::module_pointer;
    using module_ctor_t = typename base_type::module_ctor_t;
    using module_dtor_t = typename base_type::module_dtor_t;

public:
    module_pointer load_module_for_path (string_type const & path
        , std::list<string_type> const & search_dirs) override
    {
        return module_for_path(path, search_dirs.begin(), search_dirs.end());
    }

    module_pointer load_module_for_name (string_type const & basename
        , std::list<string_type> const & search_dirs) override
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

        auto dylib_ptr = std::make_shared<dynamic_library>();
        std::error_code ec;

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
#elif _MSC_VER && defined(_UNICODE)
            fprintf(stderr, "module not found: %ws\n", dylib_path.c_str());
#else
            fprintf(stderr, "module not found: %s\n", dylib_path.c_str());
#endif

            return module_pointer{nullptr, nullptr};
        }

        if (!dylib_ptr->open(dylib_path, ec)) {
            // This is a critical section, so log output must not depends on logger
#if ANDROID
            __android_log_print(ANDROID_LOG_ERROR, "modulus"
                , "open module failed: %s: %s\n"
                , dylib_path.c_str()
                , dylib_ptr->native_error().c_str());
#elif _MSC_VER && defined(_UNICODE)
            fprintf(stderr, "open module failed: %ws: %s\n"
                , dylib_path.c_str()
                , dylib_ptr->native_error().c_str());
#else
            fprintf(stderr, "open module failed: %s: %s\n"
                , dylib_path.c_str()
                , dylib_ptr->native_error().c_str());
#endif
            return module_pointer{nullptr, nullptr};
        }

        dynamic_library::symbol_type ctor = dylib_ptr->resolve(module_ctor_name, ec);

        if (!ctor) {
            this->log_error(fmt::format("{}: failed to resolve constructor `{}' for module: {}"
                , dylib_path.native()
                , string_type(module_ctor_name)
                , ec.message()));

            return module_pointer{nullptr, nullptr};
        }

        dynamic_library::symbol_type dtor = dylib_ptr->resolve(module_dtor_name, ec);

        if (!dtor) {
            this->log_error(fmt::format("{}: failed to resolve `dtor' for module: {}"
                , dylib_path.native()
                , ec.message()));

            return module_pointer{nullptr, nullptr};
        }

        module_ctor_t module_ctor = void_func_ptr_cast<module_ctor_t>(ctor);
        module_dtor_t module_dtor = void_func_ptr_cast<module_dtor_t>(dtor);

        decltype(& *module_pointer{nullptr, nullptr}) ptr = module_ctor();


        if (!ptr)
            return module_pointer{nullptr, nullptr};

        return module_pointer{ptr, module_dtor};
    }

    template <typename ForwardPathIt>
    module_pointer module_for_name (string_type const & basename
        , ForwardPathIt first, ForwardPathIt last)
    {
        auto dl_filename = fs::path::string_type(basename);
        auto modpath = dynamic_library::build_dl_filename(dl_filename);
        return module_for_path<ForwardPathIt>(modpath, first, last);
    }
};

} // namespace pfs





