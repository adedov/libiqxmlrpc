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
//  $Id: http_client.cc,v 1.2 2004-05-17 08:43:02 adedov Exp $

#include <iostream>
#include "sysinc.h"
#include "http_client.h"

using namespace iqxmlrpc;
using namespace iqnet;


Http_client_connection::Http_client_connection( const iqnet::Socket& s ):
  Client_connection( s ),
  reactor( new iqnet::Null_lock ),
  resp_packet(0)
{
  sock.set_non_blocking( true );
}


http::Packet* Http_client_connection::do_process_session( const std::string& s )
{
  out_str = s;
  reactor.register_handler( this, iqnet::Reactor::OUTPUT );
  
  do {
    int to = timeout >= 0 ? timeout*1000 : -1;
    if( !reactor.handle_events(to) )
      throw Client_timeout();
  }
  while( !resp_packet );

  return resp_packet;
}


void Http_client_connection::handle_output( bool& )
{
  int sz = send( out_str.c_str(), out_str.length() );
  out_str.erase( 0, sz );
  
  if( out_str.empty() )
  {
    reactor.unregister_handler( this, iqnet::Reactor::OUTPUT );
    reactor.register_handler( this, iqnet::Reactor::INPUT );
  }
}


void Http_client_connection::handle_input( bool& )
{
  for( int sz = read_buf_sz; (sz == read_buf_sz) && !resp_packet ; )
  {
    read_buf[0] = 0;
    int sz = recv( read_buf, read_buf_sz  );
    resp_packet = read_response( sz ? std::string(read_buf, sz) : "" );
  }
  
  if( resp_packet )
    reactor.unregister_handler( this );
}
