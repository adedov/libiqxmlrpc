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
//  $Id: http_server.cc,v 1.4 2004-10-22 04:13:27 adedov Exp $

#include <iostream>
#include "sysinc.h"
#include "http_server.h"

using namespace iqxmlrpc;
using namespace iqnet;


Http_server_connection::Http_server_connection( const iqnet::Socket& s ):
  Connection( s ),
  Server_connection( s.get_peer_addr() )
{
}
    

void Http_server_connection::post_accept()
{
  sock.set_non_blocking(true);
  reactor->register_handler( this, Reactor::INPUT );
}


void Http_server_connection::finish()
{
  delete this;
}


void Http_server_connection::handle_input( bool& terminate )
{   
  try {
    int n = recv( read_buf, read_buf_sz );
    
    if( !n )
    {
      terminate = true;
      return;
    }
    
    http::Packet* packet = read_request( std::string(read_buf, n) );
    if( !packet )
      return;
    
    reactor->unregister_handler( this, Reactor::INPUT );
    server->schedule_execute( packet, this );
  }
  catch( const http::Error_response& e )
  {
    schedule_response( new http::Packet(e) );
  }
}


void Http_server_connection::handle_output( bool& terminate )
{
  int sz = send( response.c_str(), response.length() );

  if( sz == response.length() )
  {
    terminate = true;
    return;
  }

  response.erase( 0, sz );
}


void Http_server_connection::schedule_response( http::Packet* pkt )
{
  Server_connection::schedule_response( pkt );
  reactor->register_handler( this, iqnet::Reactor::OUTPUT );
}


void Http_server_connection::log_exception( const std::exception& ex )
{
  std::string s( "iqxmlrpc::Http_server_connection: " );
  s += ex.what();
  server->log_err_msg( s );
}


void Http_server_connection::log_unknown_exception()
{
  server->log_err_msg( "iqxmlrpc::Http_server_connection: unknown exception." );
}
