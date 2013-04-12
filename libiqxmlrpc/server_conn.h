//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_server_conn_h_
#define _iqxmlrpc_server_conn_h_

#include "connection.h"
#include "conn_factory.h"
#include "http.h"

namespace iqnet
{
  class Reactor_base;
}

namespace iqxmlrpc {

class Server;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

//! Base class for XML-RPC server connections.
class LIBIQXMLRPC_API Server_connection {
protected:
  iqnet::Inet_addr peer_addr;
  Server *server;
  http::Packet_reader preader;

  size_t read_buf_sz;
  char    *read_buf;

  std::string response;

protected:
  bool keep_alive;

public:
  Server_connection( const iqnet::Inet_addr& );
  virtual ~Server_connection() = 0;

  const iqnet::Inet_addr& get_peer_addr() const { return peer_addr; }

  void set_read_sz( size_t );

  void set_server( Server* s )
  {
    server = s;
  }

  void schedule_response( http::Packet* );

protected:
  http::Packet* read_request( const std::string& );

  virtual void do_schedule_response() = 0;
};

#ifdef _MSC_VER
#pragma warning(pop)
#pragma warning(disable: 4251)
#endif

//! Server connections factory.
template < class Transport >
class Server_conn_factory: public iqnet::Serial_conn_factory<Transport>
{
  Server* server;
  iqnet::Reactor_base* reactor;

public:
  Server_conn_factory():
    server(0), reactor(0) {}

  void post_init( Server* s, iqnet::Reactor_base* r )
  {
    server = s;
    reactor = r;
  }

  void post_create( Transport* c )
  {
    c->set_server( server );
    c->set_reactor( reactor );
  }
};

} // namespace iqxmlrpc

#endif
