//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "http_server.h"
#include "server.h"
#include "server_conn.h"

using namespace iqnet;

namespace iqxmlrpc {

namespace {

//! Represents server-side \b HTTP non-blocking connection.
class Http_server_connection:
  public iqnet::Connection,
  public Server_connection
{
  iqnet::Reactor_base* reactor;

public:
  Http_server_connection( const iqnet::Socket& );

  void set_reactor( iqnet::Reactor_base* r ) { reactor = r; }

  void post_accept();
  void finish();

  void handle_input( bool& );
  void handle_output( bool& );


  bool catch_in_reactor() const { return true; }
  void log_exception( const std::exception& );
  void log_unknown_exception();

private:
  virtual void do_schedule_response();
};

typedef Server_conn_factory<Http_server_connection> Http_conn_factory;

} // anonymous namespace

//
// Http_server
//

Http_server::Http_server(const iqnet::Inet_addr& bind_addr, Executor_factory_base* ef):
  Server(bind_addr, new Http_conn_factory, ef)
{
  static_cast<Http_conn_factory*>(get_conn_factory())->post_init(this, get_reactor());
}

//
// Http_server_connection
//

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
    size_t n = recv( read_buf(), read_buf_sz() );

    if( !n )
    {
      terminate = true;
      return;
    }

    http::Packet* packet = read_request( std::string(read_buf(), n) );
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
  size_t sz = send( response.c_str(), response.length() );

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


void Http_server_connection::do_schedule_response()
{
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

} // namespace iqxmlrpc
