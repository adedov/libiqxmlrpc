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
//  $Id: http_transport.cc,v 1.10 2004-04-14 08:49:16 adedov Exp $

#include <unistd.h>
#include <iostream>
#include "http_transport.h"

using namespace iqxmlrpc;
using namespace iqnet;


Http_server_connection::Http_server_connection( int fd, const Inet_addr& addr ):
  Server_connection( fd, addr )
{
}
    

void Http_server_connection::post_accept()
{
  set_non_blocking(true);
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


// --------------------------------------------------------------------------
http::Packet* Http_client_connection::do_process_session( const std::string& s )
{
  send( s.c_str(), s.length() );
  
  for(;;)
  {
    read_buf[0] = 0;
    unsigned sz = recv( read_buf, read_buf_sz-1  );
    
    if( !sz )
      throw http::Malformed_packet();
    
    http::Packet* pkt = read_response( std::string(read_buf, sz) );
    if( pkt )
      return pkt;
  }
}
