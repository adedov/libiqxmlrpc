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
//  $Id: https_server.h,v 1.7 2006-09-04 12:13:31 adedov Exp $

#ifndef _libiqxmlrpc_https_server_h_
#define _libiqxmlrpc_https_server_h_

#include "ssl_connection.h"
#include "server_conn.h"
#include "server.h"

namespace iqxmlrpc
{

//! Represents server-side \b HTTPS non-blocking connection.
class Https_server_connection:
  public iqnet::ssl::Reaction_connection,
  public iqxmlrpc::Server_connection
{
  char* send_buf;

public:
  Https_server_connection( const iqnet::Socket& );

  void post_accept() { Reaction_connection::post_accept(); }
  void finish() { delete this; }

  void schedule_response( http::Packet* );

  bool catch_in_reactor() const { return true; }
  void log_exception( const std::exception& );
  void log_unknown_exception();

protected:
  void my_reg_recv();
  void accept_succeed();
  void recv_succeed( bool& terminate, int req_len, int real_len );
  void send_succeed( bool& terminate );
};

//! XML-RPC server that works over secured HTTP connections (HTTPS).
class Https_server: public Server {
  typedef Server_conn_factory<Https_server_connection> Conn_factory;

public:
  Https_server(int port, Executor_factory_base* ef):
    Server(port, new Conn_factory, ef)
  {
    static_cast<Conn_factory*>(conn_factory.get())->post_init(this, reactor.get());
  }
};

} // namespace iqxmlrpc

#endif
