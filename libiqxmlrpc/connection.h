//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_connection_h_
#define _libiqnet_connection_h_

#include "inet_addr.h"
#include "net_except.h"
#include "reactor.h"

#include <string>

namespace iqnet
{

//! An established TCP-connection.
/*!
    A build block for connection handling.
    Have to be created by connection's factory.
*/
class LIBIQXMLRPC_API Connection: public Event_handler {
protected:
  Socket sock;

public:
  Connection( const Socket& );
  virtual ~Connection();

  void finish();

  virtual void post_accept() {}
  virtual void post_connect() {}

  const iqnet::Inet_addr& get_peer_addr() const
  {
    return sock.get_peer_addr();
  }

  Socket::Handler get_handler() const
  {
    return sock.get_handler();
  }

  virtual int send( const char*, int );
  virtual int recv( char*, int );
};

} // namespace iqnet

#endif
