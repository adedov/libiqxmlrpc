//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

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
  size_t sz = send( out_str.c_str(), out_str.length() );
  out_str.erase( 0, sz );

  if( out_str.empty() )
  {
    reactor->unregister_handler( this, Reactor_base::OUTPUT );
    reactor->register_handler( this, Reactor_base::INPUT );
  }
}


void Http_client_connection::handle_input( bool& )
{
  for( size_t sz = read_buf_sz; (sz == read_buf_sz) && !resp_packet ; )
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
