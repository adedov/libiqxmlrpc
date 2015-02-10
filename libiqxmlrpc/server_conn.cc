//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "server_conn.h"
#include "auth_plugin.h"
#include "http_errors.h"
#include "server.h"

using namespace iqxmlrpc;

Server_connection::Server_connection( const iqnet::Inet_addr& a ):
  peer_addr(a),
  server(0),
  keep_alive(false),
  read_buf_(65536, '\0')
{
}


Server_connection::~Server_connection()
{
}


http::Packet* Server_connection::read_request( const std::string& s )
{
  try
  {
    preader.set_verification_level( server->get_verification_level() );
    preader.set_max_size( server->get_max_request_sz() );
    http::Packet* r = preader.read_request(s);

    if( r ) {
      keep_alive = r->header()->conn_keep_alive();
    } else if( preader.expect_continue() ) {
      response = "HTTP/1.1 100\r\n\r\n";
      keep_alive = true;
      do_schedule_response();
      preader.set_continue_sent();
    }

    return r;
  }
  catch( const http::Malformed_packet& )
  {
    throw http::Bad_request();
  }
}


void Server_connection::schedule_response( http::Packet* pkt )
{
  std::auto_ptr<http::Packet> p(pkt);
  p->set_keep_alive( keep_alive );
  response = p->dump();
  do_schedule_response();
}

// vim:ts=2:sw=2:et
