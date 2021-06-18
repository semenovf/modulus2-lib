////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [modulus2-lib](https://github.com/semenovf/modulus2-lib) library.
//
// Changelog:
//      2021.05.20 Initial version (inherited from https://github.com/semenovf/pfs-modulus)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/modulus2/quit_plugin.hpp"
#include "pfs/emitter.hpp"
#include "pfs/function_queue.hpp"
#include "pfs/memory.hpp"
#include <map>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <cassert>

namespace pfs {

template <typename StringType>
struct lexical_caster
{
    template <typename T>
    StringType operator () (T const & arg);
};

template <>
struct lexical_caster<std::string>
{
    template <typename T>
    std::string operator () (T const & arg)
    {
        using std::to_string;
        return to_string(arg);
    }
};

template <>
inline std::string lexical_caster<std::string>::operator () (std::string const & s)
{
    return s;
}

template <typename LoggerType
    , typename PropertiesType = std::true_type
    , typename ApiIdType = int
    , typename StringType = std::string>
struct modulus2
{
    using logger_type = LoggerType;
    using properties_type = PropertiesType;
    using api_id_type = ApiIdType;
    using string_type = StringType;

    template <typename ...Args>
    using emitter_type = emitter_mt<Args...>;
    using basic_emitter_type = emitter_mt<>;

    using function_queue_type = function_queue<>;
    using module_name_type = std::pair<string_type, string_type>;

    using thread_pool_type = std::list<std::thread>;

    enum class exit_status
    {
          success = 0
        , failure = -1
    };

////////////////////////////////////////////////////////////////////////////////
// concat
////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    static inline string_type concat (T const & arg)
    {
        return lexical_caster<string_type>{}(arg);
    }

    template <typename T, typename ...Ts>
    static inline string_type concat (T const & arg, Ts const &... args)
    {
        return lexical_caster<string_type>{}(arg) + concat(args...);
    }

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
// Runnable interface
////////////////////////////////////////////////////////////////////////////////
    class runnable_interface
    {
    protected:
        runnable_interface * self () { return this; }

    public:
        virtual exit_status run () = 0;
    };

////////////////////////////////////////////////////////////////////////////////
// basic_module
////////////////////////////////////////////////////////////////////////////////
    class basic_module // : public sigslot_ns::basic_slot_holder
    {
        friend class dispatcher;
        friend class module_context;

//     public:
//         using string_type = StringType;
//         using emitter_mapper_pair = modulus::emitter_mapper_pair;
//
//         // MSVC do not want 'detector_mapper_pair' definition in upper level, so duplicate here
//         typedef struct { int id; detector_handler detector; } detector_mapper_pair;
//         //using detector_mapper_pair = modulus::detector_mapper_pair;
//
//         using detector_handler = modulus::detector_handler;
//         using thread_function = int (basic_module::*)(settings_type const &);
//         //using thread_function = modulus::thread_function;
//
    protected:
        string_type  _name;
        dispatcher * _dispatcher_ptr {nullptr};
//         bool         _started = false;

//         /**
//          * Acquire timer with callback processed from module's queue.
//          */
//         timer_id acquire_timer (double delay
//                 , double period
//                 , typename timer_pool_type::callback_type && callback)
//         {
//             return _dispatcher_ptr->acquire_timer(this
//                     , delay
//                     , period
//                     , std::forward<typename timer_pool_type::callback_type>(callback));
//         }
//
//         /**
//          * Acquire timer with callback processed from dispatcher queue
//          */
//         timer_id acquire_timer_dispatcher (double delay
//                 , double period
//                 , typename timer_pool_type::callback_type && callback)
//         {
//             return _dispatcher_ptr->acquire_timer(delay, period
//                     , std::forward<typename timer_pool_type::callback_type>(callback));
//         }
//
//         inline void destroy_timer (timer_id id)
//         {
//             _dispatcher_ptr->destroy_timer(id);
//         }
//
    protected:
        void set_dispatcher (dispatcher * pdisp) noexcept
        {
            _dispatcher_ptr = pdisp;
        }

        void set_name (string_type const & name) noexcept
        {
            _name = name;
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

        bool is_quit () const
        {
            return _dispatcher_ptr->is_quit();
        }

        void quit ()
        {
            _dispatcher_ptr->quit();
        }

    public:
        virtual ~basic_module () {}

        string_type const & name () const noexcept
        {
            return _name;
        }

        virtual runnable_interface * runnable () { return nullptr; }

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

    protected:
        /**
         * Module's on_start() method called after loading and connection
         * completed.
         */
        virtual bool on_start (properties_type const &)
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
    }; // basic_module

    ////////////////////////////////////////////////////////////////////////////
    // module_contex
    ////////////////////////////////////////////////////////////////////////////
    class module_context
    {
        using emitter_cache_type = std::map<api_id_type, basic_emitter_type *>;
        using module_pointer = std::unique_ptr<basic_module>;

        dispatcher *       _dispather_ptr {nullptr};
        module_pointer     _module_ptr;
        string_type        _parent_name;
        emitter_cache_type _emitter_cache;

        // FIXME
//         std::shared_ptr<dynamic_library> pdl;
    public:
        using map_type = std::map<string_type, module_context>;

    private:
        void trace_emitter_connected (api_id_type id
            , string_type const & ename  // emitter owner name
            , string_type const & dname) // detector owner name
        {
            _dispather_ptr->log_trace(concat(string_type("\tEmitter [")
                , id
                , string_type("] of module [")
                , ename
                , string_type("] connected with corresponding detector of module [")
                , dname
                , string_type("]")));
        }

    public:
        module_context (dispatcher & d
                , string_type const & name
                , string_type const & parent_name
                , std::unique_ptr<basic_module> && m)
            : _dispather_ptr(& d)
            , _module_ptr(std::move(m))
            , _parent_name(parent_name)
        {
            assert(_module_ptr);
            _module_ptr->set_dispatcher(& d);
            _module_ptr->set_name(name);

            _dispather_ptr->log_trace(concat(string_type("Declare emitters for module: ")
                , this->name()));
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
            _dispather_ptr->log_trace(concat(string_type("\tCaching emitter [")
                , id
                , string_type("] for ")
                , this->name()));
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
        bool connect_detector (api_id_type id, ModuleClass & m
            , void (ModuleClass::*f) (Args...))
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

    private:
        friend class dispatcher;

        void connect_emitters (typename map_type::iterator first
            , typename map_type::iterator last)
        {
            _dispather_ptr->log_trace(string_type("Connecting emitters:"));

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

            _dispather_ptr->log_trace(concat(
                  string_type("emitters disconnected for [")
                , _module_ptr->name()
                , string_type("]")));
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

        using string_type = modulus2::string_type;
        using module_context_map_type = typename module_context::map_type;
        using thread_pool_type = std::list<std::thread>;

    public:
        // External interfaces for modulus2
        emitter_type<string_type const &> module_registered;
        emitter_type<string_type const &> module_unregistered;
        emitter_type<string_type const &> module_started;

    private:
        function_queue_type  _q;
        module_context_map_type _module_specs;
        intmax_t _wait_period {10000}; // wait period in microseconds
                                       // (default is 10 milliseconds)

        std::atomic_int _quit_flag {0};

        string_type _main_thread_module; // Contains name of the module that
                                         // must be run in "main" thread
        logger_type * _logger_ptr{nullptr};

        void (dispatcher::*_log_printer) (void (logger_type::*)(string_type const &)
            , basic_module const * m, string_type const & s) = nullptr;

    private:
        // Logger backend for direct printing
        void direct_print (void (logger_type::*log)(string_type const &)
            , basic_module const * m
            , string_type const & s)
        {
            (_logger_ptr->*log)(m ? m->name() + ": " + s : s);
        }

        // Logger backend for queued printing
        void queued_print (void (logger_type::*log)(string_type const &)
            , basic_module const * m
            , string_type const & s)
        {
            _q.push(log, _logger_ptr, (m != 0 ? m->name() + ": " + s : s));
        }

        bool register_module_helper (
              string_type const & name
            , string_type const & parent_name
            , std::unique_ptr<basic_module> && m)
        {
            // Set parent queue for guest module
            if (m->is_guest()) {
                auto module_ptr = static_cast<guest_module*>(& *m);

                // Dispatcher is a parent module
                if (parent_name.empty()) {
                    module_ptr->set_parent_queue(& this->_q);
                } else {
                    auto parent_it = _module_specs.find(parent_name);

                    if (parent_it == _module_specs.end()) {
                        log_error(concat(string_type("Parent module [")
                            , parent_name
                            , string_type("] not found for module [")
                            , name
                            , string_type("]")));
                        return false;
                    }

                    auto q = parent_it->second.module()->queue();

                    if (! q) {
                        log_error(concat(string_type("Parent module [")
                            , parent_name
                            , string_type("] is not runnable or a guest module")));
                        return false;
                    }

                    module_ptr->set_parent_queue(q);
                }
            }

            module_context ctx {
                  *this
                , name
                , parent_name
                , std::forward<std::unique_ptr<basic_module>>(m)
            };

            auto ctx_it = _module_specs.find(ctx.name());

            if (ctx_it != _module_specs.end()) {
                log_error(concat(ctx.name(), string_type(": module already registered")));
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

                log_debug(concat(ctx.name(), string_type(": registered")));

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

            pos->disconnect_emitters();
            auto result = _module_specs.erase(pos);

            log_debug(concat(name, string_type(": unregistered")));

            // Unregister (recursively) children if have
            if (is_runnable) {
                for (auto it = _module_specs.begin(); it != _module_specs.end();) {
                    if (it->second.parent_name() == name) {
                        it = unregister_module_helper(it->second.parent_name());
                    } else {
                        ++it;
                    }
                }

                result = _module_specs.begin();
            }

            return result;
        }

        // Thread function for dispatcher and runnable modules
        exit_status runnable_main (string_type const & name, properties_type const & props)
        {
            auto r = exit_status::success;

            // on_start() runnable module
            if (!name.empty()) {
                auto runnable_it = _module_specs.find(name);

                if (runnable_it == _module_specs.end()) {
                    r = exit_status::failure;
                } else {
                    auto module_ptr = runnable_it->second.module();

                    assert(module_ptr);

                    if (!module_ptr->is_runnable()) {
                        log_warn(concat(string_type("Module")
                            , string_type(" [")
                            , module_ptr->name()
                            , string_type("] ")
                            , string_type("must be runnable")));
                        r = exit_status::failure;
                    } else if (!module_ptr->on_start(props)) {
                        r = exit_status::failure;
                    }
                }
            }

            // on_start() children
            if (r == exit_status::success) {
                for (auto & ctx: _module_specs) {
                    if (ctx.second.parent_name() == name) {

                        assert(ctx.second.module());

                        if (ctx.second.module()->is_guest()) {
                            if (!ctx.second.module()->on_start(props))
                                r = exit_status::failure;
                        }
                    }
                }
            }

            // Run
            if (r == exit_status::success) {
                if (name.empty()) {

                    // Redirect log ouput to queued printer
                    _log_printer = & dispatcher::queued_print;

                    while (! _quit_flag) {
                        _q.wait_for(_wait_period);
                        _q.call_all();
                    }

                    // Redirect log ouput to direct printer
                    _log_printer = & dispatcher::direct_print;

                } else {
                    auto runnable_it = _module_specs.find(name);
                    assert(runnable_it != _module_specs.end());
                    auto module_ptr = runnable_it->second.module();
                    assert(module_ptr);
                    assert(module_ptr->runnable());

                    r = module_ptr->runnable()->run();
                }
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

        dispatcher (logger_type & logger)
            : _logger_ptr(& logger)
            , _log_printer(& dispatcher::direct_print)
        {
//             // Initialize timer pool
//             _ptimer_pool.reset(new timer_pool_type);
        }

        virtual ~dispatcher ()
        {
            //FIXME
            //finalize(false);
        }

        void quit ()
        {
            _quit_flag.store(1);
        }

        bool is_quit () const
        {
            return (_quit_flag.load() != 0);
        }

        void attach_plugin (quit_plugin & plugin)
        {
            plugin.quit.connect(*this, & dispatcher::quit);
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

////////////////////////////////////////////////////////////////////////////////
// Register/unregister modules
////////////////////////////////////////////////////////////////////////////////
        /**
         * Register in-source defined module
         */
        template <typename ModuleClass, typename ...Args>
        bool register_module (module_name_type const & name, Args &&... args)
        {
            return register_module_helper(name.first
                , name.second
                , make_unique<ModuleClass>(std::forward<Args>(args)...));
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
                log_debug(concat(ctx.second.module()->name()
                    , string_type(": unregistered")));
            }

            _module_specs.clear();
        }

////////////////////////////////////////////////////////////////////////////////
// Main execution loop
////////////////////////////////////////////////////////////////////////////////
        exit_status exec (properties_type const & props = properties_type{})
        {
            auto r = exit_status::success;
            thread_pool_type thread_pool;

            // Check if "main" module exists
            if (!_main_thread_module.empty()) {
                if (_module_specs.find(_main_thread_module) == _module_specs.end()) {
                    log_error(concat(string_type("Module [")
                        , _main_thread_module
                        , string_type("] specified as \"main\" module not found")));
                    return exit_status::failure;
                }
            }

            // If has module to be run in "main" thread, run dispatcher in
            // self thread
            if (!_main_thread_module.empty()) {
                thread_pool.emplace_back(& dispatcher::runnable_main
                    , this
                    , string_type("")
                    , props);
            }

            for (auto & ctx: _module_specs) {
                auto module_ptr = ctx.second.module();

                if (module_ptr->runnable()) {
                    log_trace(concat(string_type("Module [")
                        , module_ptr->name()
                        , string_type("] is runnable")));

                    if (_main_thread_module == module_ptr->name()) {
                        log_trace(concat(string_type("Module [")
                            , module_ptr->name()
                            , string_type("] will be run in \"main\" thread")));

                        // Launching is below
                    } else {
                        thread_pool.emplace_back(& dispatcher::runnable_main
                            , this
                            , module_ptr->name()
                            , props);
                    }
                } else if (module_ptr->is_regular()) {
                    log_trace(concat(string_type("Module [")
                        , module_ptr->name()
                        , string_type("] is regular")));

                    if (!module_ptr->on_start(props))
                        r = exit_status::failure;
                } else {
                    log_trace(concat(string_type("Module [")
                        , module_ptr->name()
                        , string_type("] is guest")));
                }
            }

            // Launch dispatcher or "main" module (according to
            // _main_thread_module value)
            if (r == exit_status::success)
                r = runnable_main(_main_thread_module, props);

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

//         int exec ()
//         {
// //             auto r = exit_status::failure;
//
//             // FIXME
// //             auto success_start = start();
//
// //             if (success_start)
// //                 r = exec_main();
// //
// //             finalize(success_start);
//
//             return static_cast<int>(r);
//         }

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
        virtual function_queue_type * queue () const override
        {
            return & _q;
        }

        void call_all ()
        {
            _q.call_all();
        }

    public:
        runnable_interface * runnable () override
        {
            return runnable_interface::self();
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
    };
};

} // namespace pfs
