////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019-2021 Vladislav Trifochkin
//
// This file is part of [pfs-common](https://github.com/semenovf/pfs-common) library.
//
// Changelog:
//      2021.05.20 Initial version (inherited from https://github.com/semenovf/pfs-modulus)
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "pfs/emitter.hpp"
#include "pfs/function_queue.hpp"
#include "pfs/memory.hpp"
#include <map>
#include <string>
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

template <typename ApiIdType = int
    , typename StringType = std::string
>
struct modulus2
{
    using api_id_type = ApiIdType;
    using string_type = StringType;

    template <typename ...Args>
    using emitter_type = emitter_mt<Args...>;
    using basic_emitter_type = emitter_mt<>;

    using function_queue_type = function_queue<>;
    using module_name_type = std::pair<string_type, string_type>;

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
    class slave_module;

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

//     public:
//         void quit ()
//         {
//             _dispatcher_ptr->quit();
//         }

        void log_info (string_type const & s)
        {
            _dispatcher_ptr->log_info(this, s);
        }

        void log_debug (string_type const & s)
        {
            _dispatcher_ptr->log_debug(this, s);
        }

        void log_warn (string_type const & s)
        {
            _dispatcher_ptr->log_warn(this, s);
        }

        void log_error (string_type const & s)
        {
            _dispatcher_ptr->log_error(this, s);
        }

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
//         basic_module () noexcept : sigslot_ns::basic_slot_holder()
//         {}

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

        virtual void connect_detector (api_id_type, dispatcher &)
        {}

        // For regular module must return nullptr.
        // For runnable module must return own queue.
        // For slave module must return master (runnable or dispatcher) queue.
        virtual function_queue_type * queue () = 0;

    public:
        virtual ~basic_module () {}

        string_type const & name () const noexcept
        {
            return _name;
        }

//         bool is_registered () const noexcept
//         {
//             return _dispatcher_ptr != 0 ? true : false;
//         }
//
//         bool is_started () const noexcept
//         {
//             return _started;
//         }
//
//         dispatcher * get_dispatcher () noexcept
//         {
//             return _dispatcher_ptr;
//         }
//
//         dispatcher const * get_dispatcher () const noexcept
//         {
//             return _dispatcher_ptr;
//         }
//
            // DEPRECATED
//         virtual emitter_mapper_pair const * get_emitters (int & count)
//         {
//             count = 0;
//             return nullptr;
//         }
            // DEPRECATED
//         virtual detector_mapper_pair const * get_detectors (int & count)
//         {
//             count = 0;
//             return nullptr;
//         }
//
//         bool is_quit () const
//         {
//             return _dispatcher_ptr->is_quit();
//         }

    protected:
        /**
         * Module's on_loaded() method called after loaded and before
         * registration (connecting emitters with detectors).
         */
        virtual bool on_loaded ()
        {
            return true;
        }

//         /**
//          * Module's on_start() method called after loaded and connection completed.
//          */
//         virtual bool on_start (settings_type const &)
//         {
//             return true;
//         }
//
//         virtual bool on_finish ()
//         {
//             return true;
//         }
//
//         bool on_start_wrapper (settings_type const & settings)
//         {
//             _started = this->on_start(settings);
//
//             if (! _started) {
//                 _dispatcher_ptr->log_error(concat(_name
//                     , string_type(": failed to start module")));
//             }
//
//             return _started;
//         }
//
//         bool on_finish_wrapper ()
//         {
//             if (! this->on_finish()) {
//                 _dispatcher_ptr->log_warn(concat(_name
//                     , string_type(": failed to finalize module")));
//             }
//
//             return true;
//         }
    }; // basic_module

    ////////////////////////////////////////////////////////////////////////////
    // module_contex
    ////////////////////////////////////////////////////////////////////////////
    class module_context
    {
        //friend class dispatcher;

        using emitter_cache_type = std::map<api_id_type, basic_emitter_type *>;

        dispatcher *                  _dispather_ptr {nullptr};
        std::unique_ptr<basic_module> _module_ptr;
        string_type                   _parent_name;
        emitter_cache_type            _emitter_cache;

        // FIXME
//         std::shared_ptr<dynamic_library> pdl;
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
            _module_ptr->declare_emitters(*this);
        }

        string_type const & name () const
        {
            return _module_ptr->name();
        }

        template <typename ...Args>
        void declare_emitter (api_id_type id, emitter_type<Args...> & em)
        {
            std::cout << "-- Caching emitter [" << id << "] for " << _module_ptr->name() << "\n";
            _emitter_cache.emplace(id, reinterpret_cast<basic_emitter_type *>(& em));
        }
    }; // module_context

    ////////////////////////////////////////////////////////////////////////////
    // dispatcher
    ////////////////////////////////////////////////////////////////////////////
    class dispatcher
    {
        friend class basic_module;
        friend class regular_module;
        friend class runnable_module;
        friend class slave_module;

        using string_type = modulus2::string_type;
        using module_context_container_type = std::map<string_type, module_context>;
        //using emitter_multimap_type = std::multimap<api_id_type, basic_emitter_type *>;

    public:
        enum class exit_status
        {
              success = 0
            , failure = -1
        };

        // External interfaces for modulus2
        emitter_type<string_type const &> module_registered;
        emitter_type<string_type const &> module_unregistered;
        emitter_type<string_type const &> module_started;

    private:
        function_queue_type  _q;
        module_context_container_type _module_specs;

        //std::unique_ptr<emitter_multimap_type> _cached_emitters;

        void (dispatcher::*info_printer) (basic_module const * m, string_type const & s);
        void (dispatcher::*debug_printer) (basic_module const * m, string_type const & s);
        void (dispatcher::*warn_printer) (basic_module const * m, string_type const & s);
        void (dispatcher::*error_printer) (basic_module const * m, string_type const & s);

    private:
        void direct_print_info (basic_module const * m, string_type const & s)
        {
            // FIXME
            //_plog->info(m != 0 ? m->name() + ": " + s : s);
        }

        void direct_print_debug (basic_module const * m, string_type const & s)
        {
            // FIXME
            //_plog->debug(m != 0 ? m->name() + ": " + s : s);
        }

        void direct_print_warn (basic_module const * m, string_type const & s)
        {
            // FIXME
            //_plog->warn(m != 0 ? m->name() + ": " + s : s);
        }

        void direct_print_error (basic_module const * m, string_type const & s)
        {
            // FIXME
            //_plog->error(m != 0 ? m->name() + ": " + s : s);
        }

        void queued_print_info (basic_module const * m, string_type const & s)
        {
            // FIXME
            //this->_queue_ptr->push(& logger_type::info
            //        , _plog
            //        , (m != 0 ? m->name() + ": " + s : s));
        }

        void queued_print_debug (basic_module const * m, string_type const & s)
        {
            // FIXME
//             this->_queue_ptr->push(& logger_type::debug
//                     , _plog
//                     , (m != 0 ? m->name() + ": " + s : s));
        }

        void queued_print_warn (basic_module const * m, string_type const & s)
        {
            // FIXME
//             this->_queue_ptr->push(& logger_type::warn
//                     , _plog
//                     , (m != 0 ? m->name() + ": " + s : s));
        }

        void queued_print_error (basic_module const * m, string_type const & s)
        {
            // FIXME
//             this->_queue_ptr->push(& logger_type::error
//                     , _plog
//                     , (m != 0 ? m->name() + ": " + s : s));
        }

//         void connect_all ()
//         {
//             for (auto & modspec: _module_specs) {
//                 auto * pmodule = & *modspec.second.pmodule;
//
//                 for (auto & em: *_emitters_mapping) {
//                     auto id = em.first;
//                     pmodule->connect_detectors(id, *this);
//                 }
//             }
//
//             // Emitters mapping is no longer needed
//             _emitters_mapping.reset();
//         }

        bool register_module_helper (module_context && ctx)
        {
            // Cache module emitters
//             if (!_cached_emitters) {
//                 _cached_emitters.reset(new emitter_multimap_type);
//             }
//
// //             int nemitters, ndetectors;

//             auto registering_module_ptr = ctx.module();
//             auto const & module_name = ctxname.first;
//             auto const & dep_module_name = name.second;

            auto ctx_it = _module_specs.find(ctx.name());

            if (ctx_it != _module_specs.end()) {
                log_error(concat(ctx.name(), string_type(": module already registered")));
                return false;
            }

//             for (auto & modspec: _module_specs) {
//                 auto * module_ptr = & *modspec.second.pmodule;
//
// //                 for (api_id_type id: api_traits) {
// //                     std::cout << "Id: " << id << "\n";
// // //                 for (auto & em: *_emitters_mapping) {
// // //                     auto id = em.first;
// // //                     pmodule->connect_detectors(id, *this);
// // //                 }
// //                 }
//             }

//             // FIXME
// //             if (pmodule->use_queued_slots()) {
// //                 this->_runnable_modules.emplace_back(std::make_pair(& *modspec.pmodule
// //                     , static_cast<typename async_module::thread_function>(
// //                         & async_module::thread_function_wrapper)));
// //             } else {
// //                 if (pmodule->is_slave()) {
// //                     // Master is dispatcher
// //                     if (dep_module_name == "") {
// //                         std::static_pointer_cast<slave_module>(modspec.pmodule)
// //                             ->set_master(this);
// //                     } else {
// //                         basic_module * master = this->find_registered_module(dep_module_name);
// //
// //                         if (!master) {
// //                             log_error(concat(dep_module_name
// //                                 , string_type(": module not found")));
// //                             return false;
// //                         }
// //
// //                         if (!master->use_queued_slots()) {
// //                             log_error(concat(dep_module_name
// //                                 , string_type(": module must be asynchronous")));
// //                             return false;
// //                         }
// //
// //                         std::static_pointer_cast<slave_module>(modspec.pmodule)
// //                             ->set_master(static_cast<async_module *>(master));
// //                     }
// //                 }
// //             }
//
//             if (!pmodule->on_loaded()) {
//                 log_error(concat(pmodule->name(), string_type(": on_loaded stage failed")));
//                 return false;
//             }
//
//             // FIXME
//             //pmodule->declare_emitters(*this);

            {
                auto result = _module_specs.emplace(ctx.name(), std::move(ctx));

                // Already-existing element was checked, no reason to return false
                assert(result.second);
                auto & ctx = result.first->second;

                log_debug(concat(ctx.name(), string_type(": registered")));

                // Notify external subscribers
                this->module_registered(ctx.name());
            }

            return true;
        }

    public:
    ////////////////////////////////////////////////////////////////////////////
    // Constructors, destructor
    ////////////////////////////////////////////////////////////////////////////
        dispatcher (dispatcher const &) = delete;
        dispatcher & operator = (dispatcher const &) = delete;

        dispatcher (/*api_item_type * mapper, int n
                , settings_type & settings
                , logger_type & logger*/)
            : //basic_dispatcher()
              info_printer(& dispatcher::direct_print_info)
            , debug_printer(& dispatcher::direct_print_debug)
            , warn_printer(& dispatcher::direct_print_warn)
            , error_printer(& dispatcher::direct_print_error)
//             , _quit_flag(0)
//             , _main_module_ptr(nullptr)
//             , _psettings(& settings)
//             , _plog(& logger)
        {
            // FIXME
//             assert(_plog);
//
//             // Initialize timer pool
//             _ptimer_pool.reset(new timer_pool_type);
//
//             register_api(mapper, n);
        }

        virtual ~dispatcher ()
        {
            //FIXME
            //finalize(false);
        }


    ////////////////////////////////////////////////////////////////////////////
    // Logger specific methods
    ////////////////////////////////////////////////////////////////////////////
        void log_info (basic_module const * m, string_type const & s)
        {
            (this->*info_printer)(m, s);
        }

        void log_debug (basic_module const * m, string_type const & s)
        {
            (this->*debug_printer)(m, s);
        }

        void log_warn (basic_module const * m, string_type const & s)
        {
            (this->*warn_printer)(m, s);
        }

        void log_error (basic_module const * m, string_type const & s)
        {
            (this->*error_printer)(m, s);
        }

        void log_info (string_type const & s)
        {
            log_info(nullptr, s);
        }

        void log_debug (string_type const & s)
        {
            log_debug(nullptr, s);
        }

        void log_warn (string_type const & s)
        {
            log_warn(nullptr, s);
        }

        void log_error (string_type const & s)
        {
            log_error(nullptr, s);
        }

    ////////////////////////////////////////////////////////////////////////////
    //
    ////////////////////////////////////////////////////////////////////////////
        /**
         * Number of registered modules
         */
        size_t count () const
        {
            return _module_specs.size();
        }

    ////////////////////////////////////////////////////////////////////////////
    // Register modules
    ////////////////////////////////////////////////////////////////////////////
        /**
         * Register in-source defined module
         */
        template <typename ModuleClass, typename ...Args>
        bool register_module (module_name_type const & name, Args &&... args)
        {
            module_context ctx{
                  *this
                , name.first
                , name.second
                , make_unique<ModuleClass>(std::forward<Args>(args)...)
            };

            return register_module_helper(std::move(ctx));
        }

        /**
         * Must be invoked from module's connect_detectors() overloaded method
         * for connecting specified by @a id module's detector.
         */
        template <typename ModuleClass, typename ...Args>
        void connect_detector (api_id_type id, ModuleClass & m
            , void (ModuleClass::*f) (Args...))
        {
            // FIXME
//             auto range = _emitters_mapping->equal_range(id);
//
//             for (auto it = range.first; it != range.second; ++it) {
//                 auto em = reinterpret_cast<emitter_type<Args...> *>(it->second);
//
//                 if (m.queue())
//                     em->connect(*m.queue(), m, f);
//                 else
//                     em->connect(m, f);
//             }
        }

    ////////////////////////////////////////////////////////////////////////////
    // Start
    ////////////////////////////////////////////////////////////////////////////
        bool start ()
        {
            // FIXME
//             assert(_psettings);
//
            bool ok = true;
//
//             auto first = _module_spec_map.begin();
//             auto last  = _module_spec_map.end();
//
//             // Launch on_start() method for regular modules
//             for (; first != last; ++first) {
//                 module_spec modspec = first->second;
//                 std::shared_ptr<basic_module> pmodule = modspec.pmodule;
//
//                 bool is_regular_module = !pmodule->is_slave()
//                     && !pmodule->use_queued_slots();
//
//                 if (is_regular_module) {
//                     if (! pmodule->on_start_wrapper(*_psettings))
//                        ok = false;
//                     else
//                         this->module_started(pmodule->name());
//                 }
//             }
//
//             // Redirect log ouput.
//             if (ok) {
//                 info_printer  = & dispatcher::async_print_info;
//                 debug_printer = & dispatcher::async_print_debug;
//                 warn_printer  = & dispatcher::async_print_warn;
//                 error_printer = & dispatcher::async_print_error;
//             }

            return ok;
        }

    ////////////////////////////////////////////////////////////////////////////
    // Main execution loop
    ////////////////////////////////////////////////////////////////////////////
        int exec ()
        {
            auto r = exit_status::failure;

            // Connect detectors to declared emitters
            // connect_all();

            // FIXME
            auto success_start = start();

//             if (success_start)
//                 r = exec_main();
//
//             finalize(success_start);

            return static_cast<int>(r);
        }

    }; // dispatcher

////////////////////////////////////////////////////////////////////////////////
// Regular module
////////////////////////////////////////////////////////////////////////////////
    class regular_module: public basic_module
    {
        friend class dispatcher;

    protected:
        virtual function_queue_type * queue () override
        {
            return nullptr;
        }
    };

////////////////////////////////////////////////////////////////////////////////
// Runnable module
////////////////////////////////////////////////////////////////////////////////
    class runnable_module: public basic_module
    {
        friend class dispatcher;

        function_queue_type _q;

    protected:
        virtual function_queue_type * queue () override
        {
            return & _q;
        }
    };


////////////////////////////////////////////////////////////////////////////////
// Slave module
////////////////////////////////////////////////////////////////////////////////
    class slave_module: public basic_module
    {
        friend class dispatcher;

        // FIXME Must be set while construction according to master (runnable or dispatcher's) queue
        function_queue_type * _pq {nullptr};

    protected:
        virtual function_queue_type * queue () override
        {
            return _pq;
        }
    };
};

} // namespace pfs
