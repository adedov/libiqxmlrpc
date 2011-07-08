//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "http_server.h"
#include "server.h"

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
  reactor->register_handler( this, Reactor_base::INPUT );
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

    reactor->unregister_handler( this, Reactor_base::INPUT );
    server->schedule_execute( packet, this );
  }
  catch( const http::Error_response& e )
  {
    // Close connection after sending HTTP error response
    keep_alive = false;
    schedule_response( new http::Packet(e) );
  }
}


void Http_server_connection::handle_output( bool& terminate )
{
  unsigned sz = send( response.c_str(), response.length() );

  if( sz == response.length() )
  {
    if( keep_alive )
    {
      reactor->unregister_handler( this, Reactor_base::OUTPUT );
      reactor->register_handler( this, Reactor_base::INPUT );
    }
    else
      terminate = true;

    return;
  }

  response.erase( 0, sz );
}


void Http_server_connection::schedule_response( http::Packet* pkt )
{
  Server_connection::schedule_response( pkt );
  reactor->register_handler( this, iqnet::Reactor_base::OUTPUT );
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
