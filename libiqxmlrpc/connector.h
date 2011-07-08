//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_connector_h_
#define _libiqnet_connector_h_

#include "client_conn.h"

namespace iqnet {

class LIBIQXMLRPC_API Connector_base {
  Inet_addr peer_addr;

public:
  Connector_base( const iqnet::Inet_addr& peer );
  virtual ~Connector_base();

  //! Process connection.
  iqxmlrpc::Client_connection* connect(int timeout);

private:
  virtual iqxmlrpc::Client_connection*
  create_connection(const Socket&) = 0;
};

template <class Conn_type>
class Connector: public Connector_base {
public:
  Connector( const iqnet::Inet_addr& peer ):
    Connector_base(peer)
  {
  }

private:
  virtual iqxmlrpc::Client_connection*
  create_connection(const Socket& s)
  {
    Conn_type* c = new Conn_type( s, true );
    c->post_connect();
    return c;
  }
};

} // namespace iqnet

#endif
// vim:ts=2:sw=2:et
