////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2025 Vladislav Trifochkin
//
// License: see LICENSE file
//
// This file is part of `modulus2-lib`.
//
// Changelog:
//      2021.05.20 Initial version (inherited from https://github.com/semenovf/pfs-modulus).
//      2023.02.09 Settings is a template parameter now (not a plugin).
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "namespace.hpp"
#include <pfs/modulus2/plugins/loader.hpp>
#include <pfs/modulus2/plugins/module_lifetime.hpp>
#include <pfs/modulus2/plugins/quit.hpp>
#include <pfs/assert.hpp>
#include <pfs/emitter.hpp>
#include <pfs/error.hpp>
#include <pfs/filesystem.hpp>
#include <pfs/function_queue.hpp>
#include <pfs/i18n.hpp>
#include <pfs/memory.hpp>
#include <pfs/string_view.hpp>
#include <pfs/timer_pool.hpp>
#include <cstddef>
#include <map>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <cassert>

MODULUS2__NAMESPACE_BEGIN

/**
 * Requirements for SettingsType (see null_settings for example)
 *
 * class SettingsType {
 * public:
 *      void set (std::string const & key, INTEGRAL_TYPE value);
 *      void set (std::string const & key, FLOATING_POINT value);
 *      void set (std::string const & key, std::string const & value);
 *      void set (std::string const & key, pfs::string_view value);
 *      void set (std::string const & key, char const * value);
 *      T get (std::string const & key, T const & default_value) const;
 *      T take (std::string const & key, T const & default_value);
 *      void remove (std::string const & key);
 * };
 */

class null_settings
{
public:
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value, void>::type
    set (std::string const & key, T value) {}

    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, void>::type
    set (std::string const & key, T value) {}

    void set (std::string const & key, std::string const & value) {}
    void set (std::string const & key, pfs::string_view value) {}
    void set (std::string const & key, char const * value) {}

    template <typename T>
    T get (std::string const & key, T const & default_value) const {}

    template <typename T>
    T take (std::string const & key, T const & default_value) {}

    void remove (std::string const & key) {}
};

template <typename LoggerType
    , typename SettingsType
    , typename ApiIdType = int>
struct modulus2
{
    using logger_type = LoggerType;
    using settings_type = SettingsType;
    using api_id_type = ApiIdType;
    using string_type = std::string;
    using timer_id = pfs::timer_pool::timer_id;

    template <typename ...Args>
    using emitter_type = pfs::emitter_mt<Args...>;
    using basic_emitter_type = pfs::emitter_mt<>;

    using function_queue_type = pfs::function_queue<>;
    using module_name_type = std::pair<string_type, string_type>;

    using thread_pool_type = std::list<std::thread>;

    enum class exit_status
    {
          success = 0
        , failure = -1
    };

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
    class dispatcher;
    class module_context;
    class basic_module;
    class regular_module;
    class runnable_module;
    class guest_module;

////////////////////////////////////////////////////////////////////////////////
// Module deleter
////////////////////////////////////////////////////////////////////////////////
    struct basic_module_deleter
    {
        virtual void operator () (basic_module * m) const = 0;
        virtual ~basic_module_deleter () {}
    };

    struct default_module_deleter: public basic_module_deleter
    {
        void operator () (basic_module * m) const override
        {
            std::default_delete<basic_module>()(m);
        }

        virtual ~default_module_deleter () {}
    };

    struct dummy_module_deleter: public basic_module_deleter
    {
        void operator () (basic_module *) const override {}
        virtual ~dummy_module_deleter () {}
    };

    struct module_deleter
    {
        std::shared_ptr<basic_module_deleter> deleter;

        module_deleter (): deleter(new default_module_deleter)
        {}

        explicit module_deleter (basic_module_deleter * d)
            : deleter(d)
        {}

        explicit module_deleter (std::shared_ptr<basic_module_deleter> d)
            : deleter(d)
        {}

        // Used by static (pre-defined) module registration.
        explicit module_deleter (std::nullptr_t d)
            : deleter(new dummy_module_deleter)
        {}

        void operator () (basic_module * m) const
        {
            if (m)
                (*deleter)(m);
        }
    };

////////////////////////////////////////////////////////////////////////////////
// Runnable interface
////////////////////////////////////////////////////////////////////////////////
    class runnable_interface
    {
    protected:
        runnable_interface * self () { return this; }

    public:
        virtual exit_status run () = 0;

        // Called by dispatcher after run() execution and before on_finish()
        virtual void flush () = 0;
    };

////////////////////////////////////////////////////////////////////////////////
// basic_module
////////////////////////////////////////////////////////////////////////////////
    class basic_module // : public sigslot_ns::basic_slot_holder
    {
        friend class dispatcher;
        friend class module_context;

    protected:
        string_type _name;
        std::string _path; // May be local filesystem path, URI or any other
                           // resource identifier
        dispatcher * _dispatcher_ptr {nullptr};

    protected:
        void set_dispatcher (dispatcher * pdisp) noexcept
        {
            _dispatcher_ptr = pdisp;
        }

        dispatcher const & get_dispatcher () const noexcept
        {
            assert(_dispatcher_ptr);
            return *_dispatcher_ptr;
        }

        dispatcher & get_dispatcher () noexcept
        {
            assert(_dispatcher_ptr);
            return *_dispatcher_ptr;
        }

        void set_name (string_type const & name) noexcept
        {
            _name = name;
        }

        void set_path (std::string const & path) noexcept
        {
            _path = path;
        }

        virtual void declare_emitters (module_context &)
        {}

        virtual bool connect_detector (api_id_type, module_context &)
        {
            // There are no detectors associated with API identifier
            return false;
        }

        // For regular module must return nullptr.
        // For runnable module must return own queue.
        // For slave module must return master (runnable or dispatcher) queue.
        virtual function_queue_type * queue () const = 0;

        bool is_quit (int & status ) const
        {
            return _dispatcher_ptr->is_quit(status);
        }

        bool is_quit () const
        {
            return _dispatcher_ptr->is_quit();
        }

        void quit (int status = -1)
        {
            _dispatcher_ptr->quit(status);
        }

    public:
        virtual ~basic_module () {}

        string_type const & name () const noexcept
        {
            return _name;
        }

        std::string const & path () const noexcept
        {
            return _path;
        }

        virtual runnable_interface * runnable () { return nullptr; }

        // Returns true if module is guest (if the module is runnable or
        // dispatcher dependent)
        virtual bool is_guest () const noexcept
        {
            return false;
        }

        bool is_runnable () const noexcept
        {
            return this->queue() != nullptr && !this->is_guest();
        }

        bool is_regular () const noexcept
        {
            return this->queue() == nullptr;
        }

        void log_trace (string_type const & s)
        {
            _dispatcher_ptr->log_trace(this, s);
        }

        void log_debug (string_type const & s)
        {
            _dispatcher_ptr->log_debug(this, s);
        }

        void log_info (string_type const & s)
        {
            _dispatcher_ptr->log_info(this, s);
        }

        void log_warn (string_type const & s)
        {
            _dispatcher_ptr->log_warn(this, s);
        }

        void log_error (string_type const & s)
        {
            _dispatcher_ptr->log_error(this, s);
        }

        auto settings () -> decltype(this->get_dispatcher().settings())
        {
            return this->get_dispatcher().settings();
        }

        auto settings () const -> decltype(this->get_dispatcher().settings())
        {
            return this->get_dispatcher().settings();
        }

        template <typename F, typename ...Args>
        void enqueue (F && f, Args &&... args)
        {
            auto * q = this->queue();

            if (q)
                q->push(std::forward<F>(f), std::forward<Args>(args)...);
        }

    protected:
        /**
         * Module's on_start() method called after loading and connection
         * completed.
         */
        virtual bool on_start ()
        {
            return true;
        }

        /**
         * Finalize module.
         */
        virtual bool on_finish ()
        {
            return true;
        }

        // NOTE! This method call is direct for all types of modules, so
        // thread-safety needs to be considered while reimplemening this method.
        virtual void on_about_quit ()
        {}
    }; // basic_module

    using module_pointer = std::unique_ptr<basic_module, module_deleter>;

    ////////////////////////////////////////////////////////////////////////////
    // module_contex
    ////////////////////////////////////////////////////////////////////////////
    class module_context
    {
        using emitter_cache_type = std::map<api_id_type, basic_emitter_type *>;

        dispatcher *       _dispatcher_ptr {nullptr};
        module_pointer     _module_ptr;
        string_type        _parent_name;
        emitter_cache_type _emitter_cache;

    public:
        using map_type = std::map<string_type, module_context>;

    private:
        void trace_emitter_connected (api_id_type id
            , string_type const & ename  // emitter owner name
            , string_type const & dname) // detector owner name
        {
            _dispatcher_ptr->log_trace(tr::f_("\tEmitter [{}] of module [{}]"
                " connected with corresponding detector of module [{}]"
                , id, ename, dname));
        }

    public:
        module_context (dispatcher & d
                , string_type const & name
                , string_type const & parent_name
                , string_type const & path
                , module_pointer && m)
            : _dispatcher_ptr(& d)
            , _module_ptr(std::move(m))
            , _parent_name(parent_name)
        {
            assert(_module_ptr);
            _module_ptr->set_dispatcher(& d);
            _module_ptr->set_name(name);
            _module_ptr->set_path(path);

            _dispatcher_ptr->log_trace(tr::f_("Declare emitters for module: {}", this->name()));
            _module_ptr->declare_emitters(*this);
        }

        string_type const & name () const
        {
            return _module_ptr->name();
        }

        string_type const & parent_name () const
        {
            return _parent_name;
        }

        string_type const & path () const
        {
            return _module_ptr->path();
        }

        basic_module * module ()
        {
            return & *_module_ptr;
        }

        /**
         * Must be invoked from module's declare_emitters() overloaded method
         * for declaring specified by @a id module's emitter.
         */
        template <typename ...Args>
        void declare_emitter (api_id_type id, emitter_type<Args...> & em)
        {
            _dispatcher_ptr->log_trace(tr::f_("\tCaching emitter [{}] for {}", id, this->name()));
            _emitter_cache.emplace(id, reinterpret_cast<basic_emitter_type *>(& em));
        }

        /**
         * Must be invoked from module's connect_detectors() overloaded method
         * for connecting specified by @a id module's detector.
         *
         * @return @c true if emitter with associated API identifier @a id found
         *         and connected to detector, @c false if otherwise.
         */
        template <typename ModuleClass, typename ...Args>
        bool connect_detector (api_id_type id, ModuleClass & m, void (ModuleClass::*f) (Args...))
        {
            auto it = _emitter_cache.find(id);

            if (it != _emitter_cache.end()) {
                auto em = reinterpret_cast<emitter_type<Args...> *>(it->second);

                if (m.queue())
                    em->connect(*m.queue(), m, f);
                else
                    em->connect(m, f);

                return true;
            }

            return false;
        }

        template <typename F>
        struct __emitter_traits: __emitter_traits<decltype(& F::operator())> {};

        template <typename R, typename C, typename ...Args>
        struct __emitter_traits<R (C::*) (Args...) const>
        {
            using type = emitter_type<Args...>;
        };

        template <typename ModuleClass, typename F>
        bool connect_detector (api_id_type id, ModuleClass & m, F f)
        {
            auto it = _emitter_cache.find(id);

            if (it != _emitter_cache.end()) {
                using emitter_traits = __emitter_traits<F>;

                auto em = reinterpret_cast<typename emitter_traits::type *>(it->second);

                if (m.queue())
                    em->connect(*m.queue(), f);
                else
                    em->connect(f);

                return true;
            }

            return false;
        }


    private:
        friend class dispatcher;

        void connect_emitters (typename map_type::iterator first, typename map_type::iterator last)
        {
            _dispatcher_ptr->log_trace(tr::_("Connecting emitters:"));

            // Connecting emitters of this module with corresponding detectors
            // of modules in range [first; last) and with own detectors
            for (auto & em: _emitter_cache) {
                auto id = em.first;

                // Connect with own detector
                if (_module_ptr->connect_detector(id, *this)) {
                    trace_emitter_connected(id, _module_ptr->name(), _module_ptr->name());
                }

                for (typename map_type::iterator it = first; it != last; ++it) {
                    if (it->second._module_ptr->connect_detector(id, *this)) {
                        trace_emitter_connected(id, _module_ptr->name(), it->second.name());
                    }
                }
            }

            // Connecting emitters of modules in range [first; last)
            // with corresponding detectors of this module
            for (typename map_type::iterator it = first; it != last; ++it) {
                for (auto & em: it->second._emitter_cache) {
                    auto id = em.first;

                    if (_module_ptr->connect_detector(id, it->second)) {
                        trace_emitter_connected(id, it->second.name(), _module_ptr->name());
                    }
                }
            }
        }

    public:
        void disconnect_emitters ()
        {
            for (auto & em: _emitter_cache) {
                em.second->disconnect_all();
            }

            _dispatcher_ptr->log_trace(tr::f_("emitters disconnected for [{}]", _module_ptr->name()));
        }

    }; // module_context

    ////////////////////////////////////////////////////////////////////////////
    // dispatcher
    ////////////////////////////////////////////////////////////////////////////
    class dispatcher final
    {
        friend class basic_module;
        friend class regular_module;
        friend class runnable_module;
        friend class guest_module;

        using timer_pool_type = pfs::timer_pool;
        using string_type = modulus2::string_type;
        using module_context_map_type = typename module_context::map_type;
        using thread_pool_type = std::list<std::thread>;

    ////////////////////////////////////////////////////////////////////////////
    // Plugins' specific data, signals and slots
    ////////////////////////////////////////////////////////////////////////////
    public:
        ////////////////////////////////////////////////////////////////////////
        // Module's lifetime specific data, signals and slots
        ////////////////////////////////////////////////////////////////////////
        emitter_type<string_type const &> module_about_to_register;
        emitter_type<string_type const &> module_registered;
        emitter_type<string_type const &> module_unregistered;
        emitter_type<string_type const &> module_started;

    private:
        ////////////////////////////////////////////////////////////////////////
        // Loader specific data, signals and slots
        ////////////////////////////////////////////////////////////////////////
        std::vector<loader_plugin<modulus2> *> _loaders;

        mutable function_queue_type      _q;
        std::unique_ptr<timer_pool_type> _timer_pool_ptr;
        module_context_map_type          _module_specs;

        intmax_t _wait_period {500000}; // wait period in microseconds
                                       // (default is 500 milliseconds)

        std::atomic_int _quit_flag {0};

        string_type _main_thread_module; // Contains name of the module that
                                         // must be run in "main" thread
        logger_type _logger;
        settings_type _settings;

        // If true, the dispatcher ignores modules that return false in the on_start() method.
        // Main runnable module cannot be ignored.
        bool _ignore_module_on_start_failure {false};

        void (dispatcher::*_log_printer) (void (logger_type::*)(string_type const &)
            , basic_module const * m, string_type const & s) = nullptr;

        // Contains values passed to main () function.
        int _argc {0};
        char ** _argv {nullptr};

    private:
        struct timer_callback_helper
        {
            pfs::timer_pool::callback_type callback;
            function_queue_type * callback_queue {nullptr};
            pfs::timer_pool::timer_id timerid {0};

            void operator () ()
            {
                if (callback_queue) {
                    callback_queue->push(callback);
                } else {
                    callback();
                }
            }
        };

        /**
         * Acquire periodic timer with callback processed from module's queue,
         * or processed from dispatcher's queue or called directly otherwise.
         * Do not need to destroy timer explicitly unless want to stop a timer.
         */
        inline pfs::timer_pool::timer_id start_periodic_timer (
              function_queue_type * callback_queue
            , std::chrono::milliseconds period
            , typename timer_pool_type::callback_type && callback)
        {
            if (_timer_pool_ptr) {
                timer_callback_helper timer_callback;
                timer_callback.callback_queue = callback_queue;
                timer_callback.callback = std::move(callback);
                timer_callback.timerid = _timer_pool_ptr->create(period, period, std::move(timer_callback));

                return timer_callback.timerid;
            }

            return pfs::timer_pool::timer_id{0};
        }

        /**
         * Acquire single shot timer with callback processed from module's
         * queue, or processed from dispatcher's queue or called directly
         * otherwise.
         * Do not need to destroy timer explicitly unless want to cancel a
         * timeout that has not fired yet
         */
        inline pfs::timer_pool::timer_id start_timer (
              function_queue_type * callback_queue
            , std::chrono::milliseconds timeout
            , typename timer_pool_type::callback_type && callback)
        {
            if (_timer_pool_ptr) {
                timer_callback_helper timer_callback;
                timer_callback.callback_queue = callback_queue;
                timer_callback.callback = std::move(callback);
                timer_callback.timerid = _timer_pool_ptr->create(timeout, std::move(timer_callback));

                return timer_callback.timerid;
            }

            return pfs::timer_pool::timer_id{0};
        }

        inline void destroy_timer (pfs::timer_pool::timer_id & id)
        {
            // _timer_pool_ptr may be already destroyed (i.e. on finalize())
            if (_timer_pool_ptr) {
                _timer_pool_ptr->destroy(id);
                id = 0;
            }
        }

    private:
        function_queue_type * queue () const
        {
            return & _q;
        }

        // Logger backend for direct printing
        void direct_print (void (logger_type::*log)(string_type const &)
            , basic_module const * m
            , string_type const & s)
        {
            (_logger.*log)(m ? m->name() + ": " + s : s);
        }

        // Logger backend for queued printing
        void queued_print (void (logger_type::*log)(string_type const &)
            , basic_module const * m
            , string_type const & s)
        {
            _q.push(log, & _logger, (m != 0 ? m->name() + ": " + s : s));
        }

        bool register_module_helper (
              string_type const & name
            , string_type const & parent_name
            , string_type const & path
            , module_pointer && m)
        {
            this->module_about_to_register(name);

            // Set parent queue for guest module
            if (m->is_guest()) {
                auto module_ptr = static_cast<guest_module*>(& *m);

                // Dispatcher is a parent module
                if (parent_name.empty()) {
                    module_ptr->set_parent_queue(& this->_q);
                } else {
                    auto parent_it = _module_specs.find(parent_name);

                    if (parent_it == _module_specs.end()) {
                        log_error(tr::f_("Parent module [{}] not found for module [{}]"
                            , parent_name, name));
                        return false;
                    }

                    auto q = parent_it->second.module()->queue();

                    if (! q) {
                        log_error(tr::f_("Parent module [{}] is not runnable or a guest module"
                            , parent_name));
                        return false;
                    }

                    module_ptr->set_parent_queue(q);
                }
            }

            module_context ctx {*this, name, parent_name, path
                , std::forward<module_pointer>(m)};

            auto ctx_it = _module_specs.find(ctx.name());

            if (ctx_it != _module_specs.end()) {
                log_error(tr::f_("{}: module already registered", ctx.name()));
                return false;
            }

            // Cross-connecting emitters of just registering module and already
            // registered modules.
            // If _module_specs is empty so emitters of just registering module
            // will be connected with own detectors only.
            ctx.connect_emitters(_module_specs.begin(), _module_specs.end());

            // Emplace module into module specs container
            {
                auto emplaced_module = _module_specs.emplace(ctx.name(), std::move(ctx));

                // Already-existing element was checked, no reason to return false
                assert(emplaced_module.second);
                auto & ctx = emplaced_module.first->second;

                log_debug(tr::f_("{}: registered ({})", ctx.name(), ctx.path()));

                // Notify external subscribers
                this->module_registered(ctx.name());
            }

            return true;
        }

        typename module_context_map_type::iterator
        unregister_module_helper (typename module_context_map_type::iterator pos)
        {
            auto module_ptr = pos->second.module();
            auto is_runnable = module_ptr->runnable();
            auto name = module_ptr->name();

            pos->second.disconnect_emitters();
            auto result = _module_specs.erase(pos);

            log_debug(tr::f_("{}: unregistered", name));

            this->module_unregistered(name);

            // Unregister (recursively) children if have
            if (is_runnable) {
                for (auto it = _module_specs.begin(); it != _module_specs.end();) {
                    if (it->second.parent_name() == name) {
                        it = unregister_module_helper(it/*->second.parent_name()*/);
                    } else {
                        ++it;
                    }
                }

                result = _module_specs.begin();
            }

            return result;
        }

        // Thread function for dispatcher and runnable modules
        exit_status runnable_main (string_type const & name)
        {
            auto r = exit_status::success;

            // on_start() runnable module
            if (!name.empty()) {
                auto runnable_it = _module_specs.find(name);

                if (runnable_it == _module_specs.end()) {
                    r = exit_status::failure;
                } else {
                    auto module_ptr = runnable_it->second.module();

                    PFS__ASSERT(module_ptr != nullptr, "");

                    if (!module_ptr->is_runnable()) {
                        log_error(tr::f_("Module [{}] must be runnable", module_ptr->name()));
                        r = exit_status::failure;
                    } else {
                        if (!module_ptr->on_start()) {
                            r = exit_status::failure;
                            log_error(tr::f_("module [{}] start failure", module_ptr->name()));
                        } else {
                            log_trace(tr::f_("Module [{}] started successfully", module_ptr->name()));
                        }
                    }
                }
            }

            // on_start() children
            if (r == exit_status::success) {
                std::vector<std::string> on_start_failure_modules;

                for (auto & ctx: _module_specs) {
                    if (ctx.second.parent_name() == name) {

                        auto module_ptr = ctx.second.module();

                        PFS__ASSERT(module_ptr != nullptr, "");

                        if (module_ptr->is_guest()) {
                            if (!module_ptr->on_start()) {
                                if (_ignore_module_on_start_failure) {
                                    log_error(tr::f_("module [{}] start failure, ignored by settings"
                                        " (ignore_module_on_start_failure)", module_ptr->name()));

                                    on_start_failure_modules.push_back(module_ptr->name());
                                } else {
                                    log_error(tr::f_("module [{}] start failure", module_ptr->name()));
                                    r = exit_status::failure;
                                    break;
                                }
                            } else {
                                log_trace(tr::f_("Module [{}] started successfully", module_ptr->name()));
                            }
                        }
                    }
                }

                for (auto const & modname: on_start_failure_modules)
                    unregister_module(modname);
            }

            // Run
            if (r == exit_status::success) {
                // Dispatcher
                if (name.empty()) {
                    // Redirect log ouput to queued printer
                    _log_printer = & dispatcher::queued_print;

                    while (! _quit_flag) {
                        _q.wait_for(_wait_period);
                        _q.call_all();
                    }

                    // Clear timers and destroy timer pool
                    _timer_pool_ptr->destroy_all();
                    _timer_pool_ptr.reset();

                    // Force call of pending callbacks
                    _q.call_all();

                    // Redirect log ouput to direct printer
                    _log_printer = & dispatcher::direct_print;

                } else {
                    auto runnable_it = _module_specs.find(name);
                    assert(runnable_it != _module_specs.end());
                    auto module_ptr = runnable_it->second.module();
                    assert(module_ptr);
                    assert(module_ptr->runnable());

                    r = module_ptr->runnable()->run();

                    // Force call of pending callbacks
                    module_ptr->runnable()->flush();
                }
            } else {
                _quit_flag.store(-1);
            }

            // Finalize children
            for (auto & ctx: _module_specs) {
                if (ctx.second.parent_name() == name) {
                    auto module_ptr = ctx.second.module();
                    assert(module_ptr);

                    if (module_ptr->is_guest()) {
                        if (!module_ptr->on_finish())
                            r = exit_status::failure;
                    }
                }
            }

            // Finalize parent module
            if (!name.empty()) {
                auto runnable_it = _module_specs.find(name);
                assert(runnable_it != _module_specs.end());
                auto module_ptr = runnable_it->second.module();
                assert(module_ptr);

                if (!module_ptr->on_finish())
                    r = exit_status::failure;
            }

            return r;
        }

    public:
    ////////////////////////////////////////////////////////////////////////////
    // Constructors, destructor
    ////////////////////////////////////////////////////////////////////////////
        dispatcher (dispatcher const &) = delete;
        dispatcher & operator = (dispatcher const &) = delete;
        dispatcher (dispatcher &&) = delete;
        dispatcher & operator = (dispatcher &&) = delete;

        dispatcher (logger_type && logger, settings_type && settings)
            : _logger(std::move(logger))
            , _settings(std::move(settings))
            , _log_printer(& dispatcher::direct_print)
        {}

        dispatcher (logger_type && logger)
            : _logger(std::move(logger))
            , _log_printer(& dispatcher::direct_print)
        {}

        ~dispatcher ()
        {
            unregister_all();
        }

        void ignore_module_on_start_failure (bool enable)
        {
            _ignore_module_on_start_failure = enable;
        }

        /**
         * Quit with status.
         *
         * @details @c -1 is special reserved value for normal (predefined)
         *          exit status. Developer can use any values greater than zero.
         *          @c 0 is forbidden value and interpreted as @c -1.
         */
        void quit (int status = -1)
        {
            for (auto & ctx: _module_specs) {
                basic_module * m = ctx.second.module();
                m->on_about_quit();
            }

            _quit_flag.store(status == 0 ? -1 : status);
        }

        bool is_quit (int & status) const
        {
            status = _quit_flag.load();
            return (status != 0);
        }

        bool is_quit () const
        {
            return (_quit_flag.load() != 0);
        }

        void attach_plugin (quit_plugin & plugin)
        {
            plugin.quit.connect(*this, & dispatcher::quit);
        }

        void attach_plugin (module_lifetime_plugin & plugin)
        {
            using plugin_type = module_lifetime_plugin;

            module_about_to_register.connect(plugin, & plugin_type::module_about_to_register);
            module_registered.connect(plugin, & plugin_type::module_registered);
            module_unregistered.connect(plugin, & plugin_type::module_unregistered);
        }

        void attach_plugin (loader_plugin<modulus2> & plugin)
        {
            _loaders.push_back(& plugin);
            plugin.failure.connect(*this, & dispatcher::log_error);
        }

        settings_type & settings ()
        {
            return _settings;
        }

        settings_type const & settings () const
        {
            return _settings;
        }

    ////////////////////////////////////////////////////////////////////////////
    // Logger specific methods
    ////////////////////////////////////////////////////////////////////////////
        void log_trace (basic_module const * m, string_type const & s)
        {
            (this->*_log_printer)(& logger_type::trace, m, s);
        }

        void log_debug (basic_module const * m, string_type const & s)
        {
            (this->*_log_printer)(& logger_type::debug, m, s);
        }

        void log_info (basic_module const * m, string_type const & s)
        {
            (this->*_log_printer)(& logger_type::info, m, s);
        }

        void log_warn (basic_module const * m, string_type const & s)
        {
            (this->*_log_printer)(& logger_type::warn, m, s);
        }

        void log_error (basic_module const * m, string_type const & s)
        {
            (this->*_log_printer)(& logger_type::error, m, s);
        }

        void log_trace (string_type const & s)
        {
            log_trace(nullptr, s);
        }

        void log_debug (string_type const & s)
        {
            log_debug(nullptr, s);
        }

        void log_info (string_type const & s)
        {
            log_info(nullptr, s);
        }

        void log_warn (string_type const & s)
        {
            log_warn(nullptr, s);
        }

        void log_error (string_type const & s)
        {
            log_error(nullptr, s);
        }

        /**
         * Number of registered modules
         */
        size_t count () const
        {
            return _module_specs.size();
        }

        /**
         * Set module to be run in "main" thread
         */
        void set_main_thread_module (string_type const & name)
        {
            _main_thread_module = name;
        }

        ////////////////////////////////////////////////////////////////////////
        // Register/unregister modules
        ////////////////////////////////////////////////////////////////////////
        /**
         * Register in-source defined module
         */
        template <typename ModuleClass, typename ...Args>
        bool register_module (module_name_type const & name, Args &&... args)
        {
            return register_module_helper(name.first
                , name.second
                , std::string{}
                , std::unique_ptr<ModuleClass, module_deleter>(
                      new ModuleClass(std::forward<Args>(args)...)
                    , module_deleter{}));
        }

        /**
         * Register static (pre-defined) module
         */
        template <typename ModuleClass>
        bool register_static_module (module_name_type const & name, ModuleClass * m)
        {
            return register_module_helper(name.first
                , name.second
                , std::string{}
                , std::unique_ptr<ModuleClass, module_deleter>(m, module_deleter{nullptr}));
        }

        /**
         * Register module from dynamic library (using @c loader_plugin successors)
         */
        bool register_module_for_path (module_name_type const & name
            , string_type const & path
            , std::list<fs::path> const & search_dirs = std::list<fs::path>{}
            , bundle const & args = bundle{})
        {
            bool found = false;
            bool success = false;

            for (auto & loader: _loaders) {
                auto res = loader->load_module_for_path(path, search_dirs, args);

                if (res.first) {
                    found = true;
                    success = register_module_helper(name.first
                        , name.second, res.second, std::move(res.first));
                    break;
                }
            }

            if (!found) {
                log_error(tr::f_("no module found by path: {}", path));
            }

            return found && success;
        }

        /**
         * Register module from dynamic library (using @c loader_plugin successors)
         */
        bool register_module_for_name (module_name_type const & name
            , string_type const & basename
            , std::list<fs::path> const & search_dirs = std::list<fs::path>{}
            , bundle const & args = bundle{})
        {
            bool found = false;
            bool success = false;

            for (auto & loader: _loaders) {
                auto res = loader->load_module_for_name(basename, search_dirs, args);

                if (res.first) {
                    found = true;
                    success = register_module_helper(name.first
                        , name.second, res.second, std::move(res.first));
                    break;
                }
            }

            if (!found)
                log_error(tr::f_("no module found by name: {}", basename));

            return found && success;
        }

        /**
         * Unregister module with children (if have last)
         *
         * @return @c true if module found by @a name, @c false if otherwise.
         */
        bool unregister_module (string_type const & name)
        {
            auto ctx_it = _module_specs.find(name);

            if (ctx_it == _module_specs.end())
                return false;

            unregister_module_helper(ctx_it);

            return true;
        }

        void unregister_all ()
        {
            for (auto & ctx: _module_specs) {
                ctx.second.disconnect_emitters();
                log_debug(tr::f_("{}: unregistered", ctx.second.module()->name()));
                this->module_unregistered(ctx.second.module()->name());
            }

            _module_specs.clear();
        }

////////////////////////////////////////////////////////////////////////////////
// Main execution loop
////////////////////////////////////////////////////////////////////////////////
        int argc () const noexcept
        {
            return _argc;
        }

        int & argc () noexcept
        {
            return _argc;
        }

        char ** argv () const noexcept
        {
            return _argv;
        }

        exit_status exec (int argc, char * argv[])
        {
            _argc = argc;
            _argv = argv;
            return exec();
        }

        exit_status exec ()
        {
            // Initialize timer pool
            _timer_pool_ptr = pfs::make_unique<timer_pool_type>();

            auto r = exit_status::success;
            thread_pool_type thread_pool;

            // Check if "main" module exists
            if (!_main_thread_module.empty()) {
                if (_module_specs.find(_main_thread_module) == _module_specs.end()) {
                    log_error(tr::f_("module [{}] specified as \"main\" module not found"
                        , _main_thread_module));
                    return exit_status::failure;
                }
            }

            // If has module to be run in "main" thread, run dispatcher in
            // self thread
            if (!_main_thread_module.empty()) {
                thread_pool.emplace_back(& dispatcher::runnable_main, this, string_type(""));
            }

            std::vector<std::string> on_start_failure_modules;

            for (auto & ctx: _module_specs) {
                auto module_ptr = ctx.second.module();

                if (module_ptr->runnable()) {
                    log_trace(tr::f_("module [{}] is runnable", module_ptr->name()));

                    if (_main_thread_module == module_ptr->name()) {
                        log_trace(tr::f_("module [{}] will be run in \"main\" thread"
                            , module_ptr->name()));

                        // Launching is below
                    } else {
                        thread_pool.emplace_back(& dispatcher::runnable_main, this, module_ptr->name());
                    }
                } else if (module_ptr->is_regular()) {
                    log_trace(tr::f_("module [{}] is regular", module_ptr->name()));

                    if (!module_ptr->on_start()) {
                        if (_ignore_module_on_start_failure) {
                            log_error(tr::f_("module [{}] start failure, ignored by settings"
                                " (ignore_module_on_start_failure)", module_ptr->name()));

                            on_start_failure_modules.push_back(module_ptr->name());
                        } else {
                            log_error(tr::f_("module [{}] start failure", module_ptr->name()));
                            r = exit_status::failure;
                        }
                    } else {
                        log_trace(tr::f_("module [{}] started successfully", module_ptr->name()));
                    }
                } else {
                    log_trace(tr::f_("module [{}] is guest", module_ptr->name()));
                }
            }

            for (auto const & modname: on_start_failure_modules)
                unregister_module(modname);

            // Launch dispatcher or "main" module (according to _main_thread_module value)
            if (r == exit_status::success) {
                std::string exception_string;

                try {
                    r = runnable_main(_main_thread_module);
                } catch (pfs::error const & ex) {
                    exception_string = ex.what();
                } catch (std::exception const & ex) {
                    exception_string = ex.what();
                } catch (...) {
                    exception_string = tr::_("unknown");
                }

                if (!exception_string.empty()) {
                    log_error(tr::f_("application terminating by exception: {}", exception_string));
                    this->quit();
                }
            }

            for (auto & th: thread_pool) {
                if (th.joinable())
                    th.join();
            }

            // Finalize regular modules
            for (auto & ctx: _module_specs) {
                auto module_ptr = ctx.second.module();

                if (module_ptr->is_regular()) {
                    if (!module_ptr->on_finish())
                        r = exit_status::failure;
                }
            }

            unregister_all();

            return r;
        }
    }; // dispatcher

////////////////////////////////////////////////////////////////////////////////
// Regular module
////////////////////////////////////////////////////////////////////////////////
    class regular_module: public basic_module
    {
        friend class dispatcher;
        friend class module_context;

    protected:
        virtual function_queue_type * queue () const override
        {
            return nullptr;
        }

    public:
        /**
         * Start periodic timer.
         */
        inline pfs::timer_pool::timer_id start_periodic_timer (
              std::chrono::milliseconds period
            , typename dispatcher::timer_pool_type::callback_type && callback)
        {
            // Run callback in dispatcher's queue.
            return this->_dispatcher_ptr->start_periodic_timer(
                  this->_dispatcher_ptr->queue()
                , period
                , std::move(callback));
        }

        /**
         * Start single shot timer.
         */
        inline pfs::timer_pool::timer_id start_timer (
              std::chrono::milliseconds timeout
            , typename dispatcher::timer_pool_type::callback_type && callback)
        {
            // Run callback in dispatcher's queue.
            return this->_dispatcher_ptr->start_timer(
                  this->_dispatcher_ptr->queue()
                , timeout
                , std::move(callback));
        }

        inline void destroy_timer (pfs::timer_pool::timer_id & id)
        {
            this->_dispatcher_ptr->destroy_timer(id);
        }
    };

////////////////////////////////////////////////////////////////////////////////
// Runnable module
////////////////////////////////////////////////////////////////////////////////
    class runnable_module: public basic_module
        , public runnable_interface
    {
        friend class dispatcher;
        friend class module_context;

        mutable function_queue_type _q;

    protected:
        function_queue_type * queue () const override
        {
            return & _q;
        }

        void call_all ()
        {
            _q.call_all();
        }

        void call ()
        {
            _q.call();
        }

        void call (int max_count)
        {
            _q.call(max_count);
        }

        void wait ()
        {
            _q.wait();
        }

        bool wait_for (std::chrono::microseconds microseconds)
        {
            return _q.wait_for(microseconds.count());
        }

    public:
        runnable_interface * runnable () override
        {
            return runnable_interface::self();
        }

        void flush () override
        {
            _q.call_all();
        }

    public:
        /**
         * Start periodic timer.
         */
        inline pfs::timer_pool::timer_id start_periodic_timer (
              std::chrono::milliseconds period
            , typename dispatcher::timer_pool_type::callback_type && callback)
        {
            return this->_dispatcher_ptr->start_periodic_timer(
                this->queue(), period, std::move(callback));
        }

        /**
         * Start single shot timer.
         */
        inline pfs::timer_pool::timer_id start_timer (
              std::chrono::milliseconds timeout
            , typename dispatcher::timer_pool_type::callback_type && callback)
        {
            return this->_dispatcher_ptr->start_timer(this->queue()
                , timeout, std::move(callback));
        }

        inline void destroy_timer (pfs::timer_pool::timer_id & id)
        {
            this->_dispatcher_ptr->destroy_timer(id);
        }
    };

////////////////////////////////////////////////////////////////////////////////
// Guest module
////////////////////////////////////////////////////////////////////////////////
    class guest_module: public basic_module
    {
        friend class dispatcher;
        friend class module_context;

        function_queue_type * _parent_queue {nullptr};

    protected:
        bool is_guest () const noexcept override
        {
            return true;
        }

        function_queue_type * queue () const override
        {
            return _parent_queue;
        }

        void set_parent_queue (function_queue_type * q)
        {
            _parent_queue = q;
        }

    public:
        /**
         * Start periodic timer.
         */
        inline pfs::timer_pool::timer_id start_periodic_timer (
              std::chrono::milliseconds period
            , typename dispatcher::timer_pool_type::callback_type && callback)
        {
            return this->_dispatcher_ptr->start_periodic_timer(
                this->queue(), period, std::move(callback));
        }

        /**
         * Start single shot timer.
         */
        inline pfs::timer_pool::timer_id start_timer (
              std::chrono::milliseconds timeout
            , typename dispatcher::timer_pool_type::callback_type && callback)
        {
            return this->_dispatcher_ptr->start_timer(this->queue()
                , timeout, std::move(callback));
        }

        inline void destroy_timer (pfs::timer_pool::timer_id & id)
        {
            this->_dispatcher_ptr->destroy_timer(id);
        }
    };
};

MODULUS2__NAMESPACE_END

#ifndef MODULUS2__MODULE_EXPORT
#   if _MSC_VER
#       define MODULUS2__MODULE_EXPORT __declspec(dllexport)
#   else
#       define MODULUS2__MODULE_EXPORT
#   endif
#endif // !MODULUS2__MODULE_EXPORT
