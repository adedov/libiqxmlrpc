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
//  $Id: http_transport.h,v 1.8 2004-03-29 06:23:18 adedov Exp $

#ifndef _libiqxmlrpc_http_transport_h_
#define _libiqxmlrpc_http_transport_h_

#include <libiqxmlrpc/http.h>
#include <libiqnet/acceptor.h>
#include <libiqnet/connection.h>
#include <libiqnet/conn_fabric.h>
#include <libiqnet/connector.h>

namespace iqxmlrpc
{
  class Http_reaction_connection;
  class Http_conn_fabric;
  class Http_server;
  class Http_client;
};


//! Represents server-side \b HTTP non-blocking connection.
/*! Does the actual work for sending/receiving HTTP packets via network. 
    \see iqxmlrpc::Http_server
*/
class iqxmlrpc::Http_reaction_connection: public iqnet::Connection {
  iqnet::Reactor* reactor;
  http::Packet* response;
  http::Server* server;

  std::string out_str;
  unsigned out_ptr;
  
  friend class Http_conn_fabric;
    
public:
  Http_reaction_connection( int, const iqnet::Inet_addr& );
  ~Http_reaction_connection();

  void post_accept();  
  void finish();
  
  void handle_input( bool& );
  void handle_output( bool& );
};


//! Fabric for Http_reaction_connection.
class iqxmlrpc::Http_conn_fabric: 
  public iqnet::Serial_conn_fabric<Http_reaction_connection> 
{
  Method_dispatcher *disp;
  iqnet::Reactor* reactor;

public:
  Http_conn_fabric( Method_dispatcher* d, iqnet::Reactor* r ):
    disp(d), reactor(r) {}
  
  void post_create( Http_reaction_connection* c )
  {
    c->reactor = reactor;
    c->server = new http::Server( disp );
  }
};


//! Single thread XML-RPC \b HTTP server based on reactive model.
/*! It just accepts new connections and creates objects of
    Http_reaction_connection class for each one. 
    Then Http_reaction_connection does all real work. */
class iqxmlrpc::Http_server {
  typedef Http_conn_fabric C_fabric;
  
  iqnet::Reactor   reactor;
  iqnet::Acceptor *acceptor;
  C_fabric        *cfabric;

  bool  exit_flag;

public:
  Http_server( int port, Method_dispatcher* );
  ~Http_server();

  void set_exit_flag()
  {
    exit_flag = true;
  }
  
  void work();
};


//! Single thread XML-RPC \b HTTP client based on blocking connection.
/*! Implements functions for real network collaboration,
    sending/receiving HTTP packets.
*/
class iqxmlrpc::Http_client: public iqxmlrpc::http::Client {
  iqnet::Inet_addr addr;
  iqnet::Connection* conn;
  iqnet::Connector<iqnet::Connection> ctr;
  
public:
  Http_client( const iqnet::Inet_addr&, const std::string& uri="/RPC" );
  virtual ~Http_client();

protected:
  void send_request( const http::Packet& );
  void recv_response();
};


#endif
