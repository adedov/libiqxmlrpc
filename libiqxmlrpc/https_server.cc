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
//  $Id: https_server.cc,v 1.2 2004-05-17 08:43:02 adedov Exp $

#include <iostream>
#include "https_server.h"

using namespace iqxmlrpc;
using namespace iqnet;


Https_server_connection::Https_server_connection( const iqnet::Socket& s ):
  ssl::Reaction_connection( s ),
  Server_connection( s ),
  send_buf(0)
{
}


inline void Https_server_connection::my_reg_recv()
{
  read_buf[0] = 0;
  reg_recv( read_buf, read_buf_sz-1 );
}


void Https_server_connection::accept_succeed()
{
  my_reg_recv();
}


void Https_server_connection::recv_succeed
  ( bool& terminate, int req_len, int real_len )
{
  try 
  {
    std::string s( read_buf, real_len );
    http::Packet* packet = read_request( s );

    if( !packet )
    {
      my_reg_recv();
      return;
    }

    server->schedule_execute( packet, this );
  }
  catch( const http::Error_response& e )
  {
    schedule_response( new http::Packet(e) );
  }
}


void Https_server_connection::send_succeed( bool& terminate )
{
  delete[] send_buf;
  send_buf = 0;
  terminate = reg_shutdown();
}


void Https_server_connection::schedule_response( http::Packet* pkt )
{
  Server_connection::schedule_response( pkt );

  send_buf = new char[response.length()];
  response.copy( send_buf, std::string::npos );
  reg_send( send_buf, response.length() );
}
