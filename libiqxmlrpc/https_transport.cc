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
//  $Id: https_transport.cc,v 1.7 2004-03-29 06:23:18 adedov Exp $

#include <iostream>
#include <libiqxmlrpc/https_transport.h>

using namespace iqxmlrpc;
using namespace iqnet;


Https_reaction_connection::Https_reaction_connection( 
    int fd, 
    const iqnet::Inet_addr& addr
  ):
    ssl::Reaction_connection( fd, addr ),
    server(0),
    response(0),
    recv_buf_sz(256),
    recv_buf(new char[recv_buf_sz]),
    send_buf(0)
{
}


Https_reaction_connection::~Https_reaction_connection()
{
  delete[] send_buf;
  delete[] recv_buf;
  delete response;
  delete server;
}


inline void Https_reaction_connection::my_reg_recv()
{
  bzero( recv_buf, recv_buf_sz );
  reg_recv( recv_buf, recv_buf_sz-1 );  
}


void Https_reaction_connection::accept_succeed()
{
  my_reg_recv();
}


void Https_reaction_connection::recv_succeed
  ( bool& terminate, int req_len, int real_len )
{
  try 
  {
    std::string s(recv_buf, real_len);
    if( !server->read_request( s ) )
    {
      my_reg_recv();
      return;
    }

    response = server->execute();
  }
  catch( const http::Error_response& e )
  {
    delete response;
    response = new http::Packet(e);
  }
  
  std::string s( response->dump() );
  send_buf = new char[s.length()];
  s.copy( send_buf, std::string::npos );
  reg_send( send_buf, s.length() );
}


void Https_reaction_connection::send_succeed( bool& terminate )
{
  delete[] send_buf;
  delete response;
  send_buf = 0;
  response = 0;
  terminate = reg_shutdown();
}


// --------------------------------------------------------------------------
Https_server::Https_server( int port, Method_dispatcher* disp ):
  reactor(),
  acceptor(0),
  cfabric( new C_fabric( disp, &reactor ) ),
  exit_flag(false) 
{
  acceptor = new Acceptor( port, cfabric, &reactor );
}


Https_server::~Https_server()
{
  delete acceptor;
  delete cfabric;
}


void Https_server::work()
{
  while( !exit_flag )
    reactor.handle_events();
}


// --------------------------------------------------------------------------
Https_client::Https_client( const iqnet::Inet_addr& a, const std::string& uri ):
  http::Client( uri ),
  addr(a),
  conn(0),
  ctr(a)
{
  set_client_host( iqnet::get_host_name() );
}


Https_client::~Https_client()
{
  delete conn;
}


void Https_client::send_request( const http::Packet& packet )
{
  conn = ctr.connect();
  std::string req( packet.dump() );
  conn->send( req.c_str(), req.length() );
}


void Https_client::recv_response()
{
  try {
    int sum = 0;
    
    for( bool r = false; !r; )
    {
      char buf[256];
      bzero( buf, sizeof(buf) );
      unsigned sz = conn->recv( buf, sizeof(buf) );
      r = read_response( std::string(buf, sz) );
    }
    
    conn->shutdown();
    delete conn;
    conn = 0;
  }
  catch( const ssl::connection_close& e )
  {
    if( e.is_clean() )
      conn->shutdown();

    throw http::Malformed_packet();
  }
}
