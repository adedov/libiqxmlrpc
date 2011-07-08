//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqxmlrpc_http_server_h_
#define _libiqxmlrpc_http_server_h_

#include "connector.h"
#include "inet_addr.h"
#include "server.h"
#include "server_conn.h"

namespace iqxmlrpc
{

//! Represents server-side \b HTTP non-blocking connection.
class LIBIQXMLRPC_API Http_server_connection:
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

  void schedule_response( http::Packet* packet );

  bool catch_in_reactor() const { return true; }
  void log_exception( const std::exception& );
  void log_unknown_exception();
};

//! XML-RPC server that works over plain HTTP connections.
class LIBIQXMLRPC_API Http_server: public Server {
  typedef Server_conn_factory<Http_server_connection> Conn_factory;

public:
  Http_server(const iqnet::Inet_addr& bind_addr, Executor_factory_base* ef):
    Server(bind_addr, new Conn_factory, ef)
  {
    static_cast<Conn_factory*>(get_conn_factory())->post_init(this, get_reactor());
  }
};

} // namespace iqxmlrpc

#endif
