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
//  $Id: server.h,v 1.7 2004-04-22 08:11:41 adedov Exp $

#ifndef _iqxmlrpc_server_h_
#define _iqxmlrpc_server_h_

#include <ostream>
#include "libiqnet/acceptor.h"
#include "libiqnet/connection.h"
#include "libiqnet/conn_fabric.h"
#include "executor.h"
#include "method.h"
#include "http.h"

namespace iqxmlrpc
{
  class Server_connection;
  class Server_conn_fabric;
  class Server_base;
  class Server;
};

namespace iqnet
{
  class Reactor;
};


//! Base class for XML-RPC server connections.
class iqxmlrpc::Server_connection: public iqnet::Connection {
protected:
  Server *server;
  http::Packet_reader<http::Request_header> preader;

  unsigned read_buf_sz;
  char    *read_buf;

  std::string response;

public:
  Server_connection( int sock, const iqnet::Inet_addr& peer );
  virtual ~Server_connection();

  void set_read_sz( unsigned );
  void set_server( Server* s ) { server = s; }

  virtual void schedule_response( http::Packet* );
  
protected:
  http::Packet* read_request( const std::string& );
};


//! Server connections fabric.
template < class Transport >
class iqxmlrpc::Server_conn_fabric: public iqnet::Serial_conn_fabric<Transport> 
{
  Server* server;
  iqnet::Reactor* reactor;

public:
  Server_conn_fabric( Server* s, iqnet::Reactor* r ):
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
  Executor_fabric_base* exec_fabric;

  int port;
  iqnet::Reactor reactor;
  iqnet::Accepted_conn_fabric* conn_fabric;
  iqnet::Acceptor* acceptor;

  bool exit_flag;
  std::ostream* log;

public:
  //! Construct a server.
  /*! \param port Port to accept connections on.
      \param executor_fabric Executor fabric to use to create needed Executor.
  */
  Server( int port, Executor_fabric_base* executor_fabric );
  virtual ~Server();

  //! \group Users interface
  //! \{
  //! Ask server to exit from work() event handle loop.
  void set_exit_flag() { exit_flag = true; }
  
  //! Register specific method class with server.
  template <class Method_class> void register_method( const std::string& name );
  
  //! Process accepting connections and methods dispatching.
  template <class Transport> void work();

  //! Set stream to log errors. Transfer NULL to turn loggin off.
  void log_errors( std::ostream* );
  //! \}
  
  void schedule_execute( http::Packet*, Server_connection* );
  void schedule_response( const Response&, Server_connection*, Executor* );
  
private:
  void log_err_msg( const std::string& );
};


template <class Method_class>
void iqxmlrpc::Server::register_method( const std::string& meth_name )
{
  disp.register_method( meth_name, new Method_factory<Method_class> );
}


template <class Transport>
void iqxmlrpc::Server::work()
{
  if( !conn_fabric )
  {
    conn_fabric = new Server_conn_fabric<Transport>( this, &reactor );
    acceptor = new iqnet::Acceptor( port, conn_fabric, &reactor );
  }
  
  while( !exit_flag )
    reactor.handle_events();
}

#endif
