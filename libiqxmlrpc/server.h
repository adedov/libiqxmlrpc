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
//  $Id: server.h,v 1.19 2005-03-23 18:26:00 bada Exp $

#ifndef _iqxmlrpc_server_h_
#define _iqxmlrpc_server_h_

#include <iostream>
#include <ostream>
#include "acceptor.h"
#include "connection.h"
#include "conn_factory.h"
#include "executor.h"
#include "method.h"
#include "http.h"
#include "builtins.h"

namespace iqxmlrpc
{
  class Server_connection;
  template <class Transport> class Server_conn_factory;
  class Server_base;
  class Server;
};

namespace iqnet
{
  class Reactor;
};


//! Base class for XML-RPC server connections.
class iqxmlrpc::Server_connection {
protected:
  iqnet::Inet_addr peer_addr;
  Server *server;
  http::Packet_reader<http::Request_header> preader;

  unsigned read_buf_sz;
  char    *read_buf;

  std::string response;

protected:
  bool keep_alive;  

public:
  Server_connection( const iqnet::Inet_addr& );
  virtual ~Server_connection() = 0;

  const iqnet::Inet_addr& get_peer_addr() const { return peer_addr; }
  
  void set_read_sz( unsigned );
  void set_server( Server* s ) { server = s; }

  virtual void schedule_response( http::Packet* );

protected:
  http::Packet* read_request( const std::string& );
};


//! Server connections factory.
template < class Transport >
class iqxmlrpc::Server_conn_factory: public iqnet::Serial_conn_factory<Transport> 
{
  Server* server;
  iqnet::Reactor* reactor;

public:
  Server_conn_factory( Server* s, iqnet::Reactor* r ):
    server(s), reactor(r) {}
  
  void post_create( Transport* c )
  {
    c->set_server( server );
    c->set_reactor( reactor );
  }
};


//! XML-RPC server.
class iqxmlrpc::Server {
protected:
  Method_dispatcher disp;
  Executor_factory_base* exec_factory;

  int port;
  iqnet::Reactor reactor;
  iqnet::Accepted_conn_factory* conn_factory;
  iqnet::Acceptor* acceptor;
  iqnet::Firewall_base* firewall;

  bool exit_flag;
  std::ostream* log;
  unsigned max_req_sz;

public:
  /*! \param port Port to accept connections on.
      \param executor_factory Executor factory to use to create needed Executor.
  */
  Server( int port, Executor_factory_base* executor_factory );
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
  template <class Transport> void work();

  //! Ask server to exit from work() event handle loop.
  void set_exit_flag() { exit_flag = true; }
  /*! \} */
  
  iqnet::Reactor* get_reactor() { return &reactor; }

  void schedule_execute( http::Packet*, Server_connection* );
  void schedule_response( const Response&, Server_connection*, Executor* );
  
  void log_err_msg( const std::string& );
  unsigned get_max_request_sz() const { return max_req_sz; }
};


template <class Method_class>
void iqxmlrpc::Server::register_method( const std::string& meth_name )
{
  typedef typename Method_class::Help Help;
  disp.register_method( meth_name, new Method_factory<Method_class> );
  iqxmlrpc::Introspector::register_help_obj( meth_name, new Help );
}


template <class Transport>
void iqxmlrpc::Server::work()
{
  if( !conn_factory )
  {
    conn_factory = new Server_conn_factory<Transport>( this, &reactor );
    acceptor = new iqnet::Acceptor( port, conn_factory, &reactor );
    acceptor->set_firewall( firewall );
  }
  
  while( !exit_flag )
    reactor.handle_events();
}

#endif
