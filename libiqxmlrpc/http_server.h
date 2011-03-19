//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2006 Anton Dedov
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
//  $Id: http_server.h,v 1.7 2006-09-07 04:45:21 adedov Exp $

#ifndef _libiqxmlrpc_http_server_h_
#define _libiqxmlrpc_http_server_h_

#include "connector.h"
#include "server.h"
#include "server_conn.h"

namespace iqxmlrpc
{

//! Represents server-side \b HTTP non-blocking connection.
class LIBIQXMLRPC_API Http_server_connection:
  public iqnet::Connection,
  public Server_connection
{
  iqnet::Reactor_base* reactor;

public:
  Http_server_connection( const iqnet::Socket& );

  void set_reactor( iqnet::Reactor_base* r ) { reactor = r; }

  void post_accept();
  void finish();

  void handle_input( bool& );
  void handle_output( bool& );

  void schedule_response( http::Packet* packet );

  bool catch_in_reactor() const { return true; }
  void log_exception( const std::exception& );
  void log_unknown_exception();
};

//! XML-RPC server that works over plain HTTP connections.
class LIBIQXMLRPC_API Http_server: public Server {
  typedef Server_conn_factory<Http_server_connection> Conn_factory;

public:
  Http_server(int port, Executor_factory_base* ef):
    Server(port, new Conn_factory, ef)
  {
    static_cast<Conn_factory*>(get_conn_factory())->post_init(this, get_reactor());
  }
};

} // namespace iqxmlrpc

#endif
