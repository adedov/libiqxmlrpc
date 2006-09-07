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
//  $Id: server_conn.h,v 1.3 2006-09-07 04:45:21 adedov Exp $

#ifndef _iqxmlrpc_server_conn_h_
#define _iqxmlrpc_server_conn_h_

#include "api_export.h"
#include "connection.h"
#include "conn_factory.h"
#include "http.h"

namespace iqnet
{
  class Reactor_base;
}

namespace iqxmlrpc {

class Server;

//! Base class for XML-RPC server connections.
class LIBIQXMLRPC_API Server_connection {
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

  void set_server( Server* s )
  {
    server = s;
  }

  virtual void schedule_response( http::Packet* );

protected:
  http::Packet* read_request( const std::string& );
};

//! Server connections factory.
template < class Transport >
class Server_conn_factory: public iqnet::Serial_conn_factory<Transport>
{
  Server* server;
  iqnet::Reactor_base* reactor;

public:
  Server_conn_factory():
    server(0), reactor(0) {}

  void post_init( Server* s, iqnet::Reactor_base* r )
  {
    server = s;
    reactor = r;
  }

  void post_create( Transport* c )
  {
    c->set_server( server );
    c->set_reactor( reactor );
  }
};

} // namespace iqxmlrpc

#endif
