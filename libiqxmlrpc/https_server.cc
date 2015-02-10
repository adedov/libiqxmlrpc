//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "https_server.h"
#include "server_conn.h"
#include "ssl_connection.h"

using namespace iqnet;

namespace iqxmlrpc {

namespace {

//! Represents server-side \b HTTPS non-blocking connection.
class Https_server_connection:
  public iqnet::ssl::Reaction_connection,
  public iqxmlrpc::Server_connection
{
public:
  Https_server_connection( const iqnet::Socket& );

  void post_accept() { Reaction_connection::post_accept(); }
  void finish() { delete this; }

  bool catch_in_reactor() const { return true; }
  void log_exception( const std::exception& );
  void log_unknown_exception();

protected:
  void my_reg_recv();
  void accept_succeed();
  void recv_succeed( bool& terminate, size_t req_len, size_t real_len );
  void send_succeed( bool& terminate );
  virtual void do_schedule_response();
};

typedef Server_conn_factory<Https_server_connection> Https_conn_factory;

} // anonymous namespace

//
// Https_server
//

Https_server::Https_server(const iqnet::Inet_addr& bind_addr, Executor_factory_base* ef):
  Server(bind_addr, new Https_conn_factory, ef)
{
  static_cast<Https_conn_factory*>(get_conn_factory())->post_init(this, get_reactor());
}


//
// Https_server_connection
//

Https_server_connection::Https_server_connection( const iqnet::Socket& s ):
  ssl::Reaction_connection( s ),
  Server_connection( s.get_peer_addr() )
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


void Https_server_connection::recv_succeed( bool&, size_t, size_t real_len )
{
  try
  {
    std::string s( read_buf, real_len );
    http::Packet* packet = read_request( s );

    if( !packet )
    {
      if (response.empty())
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
  response = std::string();

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
  reg_send( response.data(), response.length() );
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

} // namespace iqxmlrpc
