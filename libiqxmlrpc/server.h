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
//  $Id: server.h,v 1.3 2004-04-14 08:44:02 adedov Exp $

#ifndef _iqxmlrpc_server_h_
#define _iqxmlrpc_server_h_

#include "libiqnet/acceptor.h"
#include "libiqnet/connection.h"
#include "libiqnet/conn_fabric.h"
#include "executor.h"
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


//! Transport independent XML-RPC server class.
class iqxmlrpc::Server_base {
protected:
  Method_dispatcher* disp;
  Executor_fabric_base* exec_fabric;

  int port;
  iqnet::Reactor reactor;
  iqnet::Accepted_conn_fabric* conn_fabric;
  iqnet::Acceptor* acceptor;

  bool exit_flag;

public:
  Server_base( int port, Method_dispatcher*, Executor_fabric_base* );
  virtual ~Server_base();

  void set_exit_flag() { exit_flag = true; }
  
  virtual void work() = 0;

  void schedule_execute( http::Packet*, Server_connection* );
  void schedule_response( const Response&, Server_connection*, Executor* );
};


//! Template for concete XML-RPC server class.
template <class Transport>
class iqxmlrpc::Server: public iqxmlrpc::Server_base {
public:
  Server( int port, Method_dispatcher* disp, Executor_fabric_base* fabric ):
    Server_base( port, disp, fabric ) {}

  //! Process accepting connections and methods dispatching.
  void work();
};


template <class Transport>
void iqxmlrpc::Server<Transport>::work()
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
