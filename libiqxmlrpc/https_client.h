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

#ifndef _libiqxmlrpc_https_client_h_
#define _libiqxmlrpc_https_client_h_

#include <boost/scoped_ptr.hpp>
#include "api_export.h"
#include "client_conn.h"
#include "connection.h"
#include "ssl_connection.h"
#include "reactor.h"

namespace iqxmlrpc
{

//! XML-RPC \b HTTPS PROXY client connection.
//! DO NOT USE IT IN YOUR CODE.
class LIBIQXMLRPC_API Https_proxy_client_connection:
  public iqxmlrpc::Client_connection,
  public iqnet::Connection
{
public:
  Https_proxy_client_connection( const iqnet::Socket&, bool non_block_flag );

  void handle_input( bool& );
  void handle_output( bool& );

protected:
  http::Packet* do_process_session( const std::string& );

  void setup_tunnel();

  boost::scoped_ptr<iqnet::Reactor_base> reactor;
  boost::scoped_ptr<http::Packet> resp_packet;
  bool non_blocking;
  std::string out_str;
};

//! XML-RPC \b HTTPS client's connection.
class LIBIQXMLRPC_API Https_client_connection:
  public iqxmlrpc::Client_connection,
  public iqnet::ssl::Reaction_connection
{
  std::auto_ptr<iqnet::Reactor_base> reactor;
  http::Packet* resp_packet;
  std::string out_str;
  bool established;

public:
  typedef Https_proxy_client_connection Proxy_connection;

  Https_client_connection( const iqnet::Socket&, bool non_block_flag );

  void post_connect()
  {
    set_reactor( reactor.get() );
    iqnet::ssl::Reaction_connection::post_connect();
  }

  void connect_succeed();
  void send_succeed( bool& );
  void recv_succeed( bool&, int req_len, int real_len  );

protected:
  friend class Https_proxy_client_connection;
  http::Packet* do_process_session( const std::string& );

private:
  void reg_send_request();
};

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
