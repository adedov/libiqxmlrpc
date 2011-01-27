//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2007 Anton Dedov
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

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

#include <memory>
#include <ostream>

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
class LIBIQXMLRPC_API Server: boost::noncopyable {
protected:
  Executor_factory_base* exec_factory;

  int port;
  std::auto_ptr<iqnet::Reactor_base>          reactor;
  std::auto_ptr<iqnet::Reactor_interrupter>   interrupter;
  std::auto_ptr<iqnet::Accepted_conn_factory> conn_factory;
  std::auto_ptr<iqnet::Acceptor>              acceptor;
  iqnet::Firewall_base* firewall;

  bool exit_flag;
  std::ostream* log;
  unsigned max_req_sz;
  http::Verification_level ver_level;

private:
  Method_dispatcher_manager  disp_manager;
  std::auto_ptr<Interceptor> interceptors;
  const Auth_Plugin_base*    auth_plugin;

public:
  Server(
    int port,
    iqnet::Accepted_conn_factory* conn_factory,
    Executor_factory_base* executor_factory );

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
  void set_max_request_sz( unsigned );
  unsigned get_max_request_sz() const { return max_req_sz; }

  //! Set optional firewall object.
  void set_firewall( iqnet::Firewall_base* );

  void set_verification_level(http::Verification_level);
  http::Verification_level get_verification_level() const { return ver_level; }

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

  iqnet::Reactor_base* get_reactor() { return reactor.get(); }

  void schedule_execute( http::Packet*, Server_connection* );
  void schedule_response( const Response&, Server_connection*, Executor* );

  void log_err_msg( const std::string& );
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
