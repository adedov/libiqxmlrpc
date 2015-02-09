//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqxmlrpc_https_server_h_
#define _libiqxmlrpc_https_server_h_

#include "server.h"
#include "server_conn.h"
#include "ssl_connection.h"

namespace iqxmlrpc
{

//! Represents server-side \b HTTPS non-blocking connection.
class LIBIQXMLRPC_API Https_server_connection:
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

//! XML-RPC server that works over secured HTTP connections (HTTPS).
class LIBIQXMLRPC_API Https_server: public Server {
  typedef Server_conn_factory<Https_server_connection> Conn_factory;

public:
  Https_server(const iqnet::Inet_addr& bind_addr, Executor_factory_base* ef):
    Server(bind_addr, new Conn_factory, ef)
  {
    static_cast<Conn_factory*>(get_conn_factory())->post_init(this, get_reactor());
  }
};

} // namespace iqxmlrpc

#endif
