//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_server_h_
#define _iqxmlrpc_server_h_

#include "acceptor.h"
#include "builtins.h"
#include "connection.h"
#include "conn_factory.h"
#include "dispatcher_manager.h"
#include "executor.h"
#include "firewall.h"
#include "http.h"
#include "util.h"

namespace iqnet
{
  class Reactor_base;
  class Reactor_interrupter;
}

namespace iqxmlrpc {

class Auth_Plugin_base;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#endif

//! XML-RPC server.
class LIBIQXMLRPC_API Server {
public:
  Server(
    const iqnet::Inet_addr& addr,
    iqnet::Accepted_conn_factory* conn_factory,
    Executor_factory_base* executor_factory );

  Server(const Server&) = delete;
  Server(Server&&) = delete;
  Server& operator=(const Server&) = delete;
  Server& operator=(Server&&) = delete;

  virtual ~Server();

  //! Register method using abstract factory.
  void register_method(const std::string& name, Method_factory_base*);

  //! Push one more alternative Method Dispatcher
  //! Method Dispatchers will be used in order they added
  //! until requested method would't be found.
  //! Grabs ownership.
  void push_dispatcher(Method_dispatcher_base*);

  //! Push user defined interceptor into stack of interceptors.
  //! Grabs the ownership.
  void push_interceptor(Interceptor*);

  //! Allow clients to request introspection information
  //! via special built-in methods.
  void enable_introspection();

  //! Set stream to log errors. Transfer NULL to turn loggin off.
  void log_errors( std::ostream* );

  //! Set maximum size of incoming client's request in bytes.
  void set_max_request_sz( size_t );
  size_t get_max_request_sz() const;

  //! Set optional firewall object.
  void set_firewall( iqnet::Firewall_base* );

  void set_verification_level(http::Verification_level);
  http::Verification_level get_verification_level() const;

  void set_auth_plugin(const Auth_Plugin_base&);
  /*! \} */

  //! \name Run/stop server
  /*! \{ */
  //! Process accepting connections and methods dispatching.
  void work();

  //! Ask server to exit from work() event handle loop.
  void set_exit_flag();

  //! Interrupt poll cycle.
  void interrupt();
  /*! \} */

  iqnet::Reactor_base* get_reactor();

  void schedule_execute( http::Packet*, Server_connection* );
  void schedule_response( const Response&, Server_connection*, Executor* );

  void log_err_msg( const std::string& );

protected:
  iqnet::Accepted_conn_factory* get_conn_factory();

private:
  class Impl;
  Impl *impl;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

//! Register class Method_class as handler for call "name" with specific server.
template <class Method_class>
inline void register_method(Server& server, const std::string& name)
{
//  typedef typename Method_class::Help Help;
//  Introspector::register_help_obj( meth_name, new Help );

  server.register_method(name, new Method_factory<Method_class>);
}

//! Register function "fn" as handler for call "name" with specific server.
inline void LIBIQXMLRPC_API
register_method(Server& server, const std::string& name, Method_function fn)
{
  server.register_method(name, new Method_factory<Method_function_adapter>(fn));
}

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
