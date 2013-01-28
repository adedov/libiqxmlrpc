//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "https_server.h"
#include "server.h"

using namespace iqxmlrpc;
using namespace iqnet;


Https_server_connection::Https_server_connection( const iqnet::Socket& s ):
  ssl::Reaction_connection( s ),
  Server_connection( s.get_peer_addr() ),
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
      if (!send_buf)
        my_reg_recv();
      return;
    }

    server->schedule_execute( packet, this );
  }
  catch( const http::Error_response& e )
  {
    // Close connection after sending HTTP error response
    keep_alive = false;
    schedule_response( new http::Packet(e) );
  }
}


void Https_server_connection::send_succeed( bool& terminate )
{
  delete[] send_buf;
  send_buf = 0;

  if( keep_alive )
    my_reg_recv();
  else
    terminate = reg_shutdown();
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4996)
#endif

void Https_server_connection::do_schedule_response()
{
  send_buf = new char[response.length()];
  response.copy( send_buf, std::string::npos );
  reg_send( send_buf, response.length() );
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

void Https_server_connection::log_exception( const std::exception& ex )
{
  std::string s( "iqxmlrpc::Https_server_connection: " );
  s += ex.what();
  server->log_err_msg( s );
}


void Https_server_connection::log_unknown_exception()
{
  server->log_err_msg( "iqxmlrpc::Https_server_connection: unknown exception." );
}

// vim:et:sw=2:ts=2
