//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
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
//  
//  $Id: server.h,v 1.22 2005-09-20 16:02:59 bada Exp $

#ifndef _iqxmlrpc_server_h_
#define _iqxmlrpc_server_h_

#include <memory>
#include <ostream>
#include "acceptor.h"
#include "connection.h"
#include "conn_factory.h"
#include "executor.h"
#include "method.h"
#include "http.h"
#include "builtins.h"
#include "util.h"

namespace iqnet
{
  class Reactor_base;
};


namespace iqxmlrpc
{

//! XML-RPC server.
class iqxmlrpc::Server {
protected:
  Method_dispatcher disp;
  Executor_factory_base* exec_factory;

  int port;
  std::auto_ptr<iqnet::Reactor_base>          reactor;
  std::auto_ptr<iqnet::Accepted_conn_factory> conn_factory;
  std::auto_ptr<iqnet::Acceptor>              acceptor;
  iqnet::Firewall_base* firewall;

  bool exit_flag;
  bool soft_exit; // Soft exit in process
  std::ostream* log;
  unsigned max_req_sz;

public:
  Server( 
    int port, 
    iqnet::Accepted_conn_factory* conn_factory,
    Executor_factory_base* executor_factory );

  virtual ~Server();

  //! \name Server configuration methods
  /*! \{ */
  //! Register specific method class with server.
  template <class Method_class> 
  void register_method( const std::string& name );

  //! Allow clients to request introspection information 
  //! via special built-in methods.
  void enable_introspection();

  //! Set stream to log errors. Transfer NULL to turn loggin off.
  void log_errors( std::ostream* );

  //! Set maximum size of incoming client's request in bytes.
  void set_max_request_sz( unsigned );
  
  //! Set optional firewall object.
  void set_firewall( iqnet::Firewall_base* );
  /*! \} */

  //! \name Run/stop server
  /*! \{ */
  //! Process accepting connections and methods dispatching.
  void work();

  //! Ask server to exit from work() event handle loop.
  void set_exit_flag() { exit_flag = true; }
  /*! \} */
  
  iqnet::Reactor_base* get_reactor() { return reactor.get(); }

  void schedule_execute( http::Packet*, Server_connection* );
  void schedule_response( const Response&, Server_connection*, Executor* );
  
  void log_err_msg( const std::string& );
  unsigned get_max_request_sz() const { return max_req_sz; }

private:
  void perform_soft_exit();
};


template <class Method_class>
void iqxmlrpc::Server::register_method( const std::string& meth_name )
{
  typedef typename Method_class::Help Help;
  disp.register_method( meth_name, new Method_factory<Method_class> );
  iqxmlrpc::Introspector::register_help_obj( meth_name, new Help );
}

} // namespace iqxmlrpc

#endif
