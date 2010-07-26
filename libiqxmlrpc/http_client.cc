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

#include "http_client.h"

#include "client_opts.h"
#include "reactor_impl.h"

#include <sstream>

using namespace iqxmlrpc;
using namespace iqnet;


Http_client_connection::Http_client_connection( const iqnet::Socket& s, bool nb ):
  Client_connection(),
  Connection( s ),
  reactor( new Reactor<Null_lock> ),
  resp_packet(0)
{
  sock.set_non_blocking( nb );
}


http::Packet* Http_client_connection::do_process_session( const std::string& s )
{
  out_str = s;
  resp_packet = 0;
  reactor->register_handler( this, Reactor_base::OUTPUT );

  do {
    int to = opts().timeout() >= 0 ? opts().timeout() * 1000 : -1;
    if( !reactor->handle_events(to) )
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
    reactor->unregister_handler( this, Reactor_base::OUTPUT );
    reactor->register_handler( this, Reactor_base::INPUT );
  }
}


void Http_client_connection::handle_input( bool& )
{
  for( unsigned sz = read_buf_sz; (sz == read_buf_sz) && !resp_packet ; )
  {
    read_buf[0] = 0;

    if( !(sz = recv( read_buf, read_buf_sz )) )
      throw iqnet::network_error( "Connection closed by peer.", false );

    resp_packet = read_response( std::string(read_buf, sz) );
  }

  if( resp_packet )
    reactor->unregister_handler( this );
}

//
// Http_proxy_client_connection
//

std::string Http_proxy_client_connection::decorate_uri() const
{
  std::ostringstream ss;
  ss << "http://" << opts().vhost() << ':' << opts().addr().get_port();

  if (!opts().uri().empty() && opts().uri()[0] != '/')
    ss << '/';

  ss << opts().uri();

  return ss.str();
}

// vim:ts=2:sw=2:et
