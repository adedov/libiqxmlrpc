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
//  $Id: https_client.h,v 1.5 2004-11-14 17:24:54 adedov Exp $

#ifndef _libiqxmlrpc_https_client_h_
#define _libiqxmlrpc_https_client_h_

#include "connector.h"
#include "ssl_connection.h"
#include "client.h"

namespace iqxmlrpc
{
  class Https_client_connection;
};


//! XML-RPC \b HTTPS client's connection (in blocking mode).
class iqxmlrpc::Https_client_connection: 
  public iqxmlrpc::Client_connection,
  public iqnet::ssl::Reaction_connection
{
  iqnet::Reactor reactor;
  http::Packet* resp_packet;
  std::string out_str;
  bool established;

public:
  Https_client_connection( const iqnet::Socket&, bool non_block_flag );

  void post_connect() 
  { 
    set_reactor( &reactor );
    iqnet::ssl::Reaction_connection::post_connect(); 
  }

  void connect_succeed();
  void send_succeed( bool& );
  void recv_succeed( bool&, int req_len, int real_len  );

protected:
  http::Packet* do_process_session( const std::string& );

private:
  void reg_send_request();
};


#endif
