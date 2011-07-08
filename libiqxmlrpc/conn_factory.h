//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_connection_factory_
#define _libiqnet_connection_factory_

#include "inet_addr.h"

namespace iqnet
{

//! Abstract factory for accepted connections.
class LIBIQXMLRPC_API Accepted_conn_factory {
public:
  virtual ~Accepted_conn_factory() {}
  virtual void create_accepted( const Socket& ) = 0;
};


//! Factory class for single threaded connections.
template <class Conn_type>
class Serial_conn_factory: public Accepted_conn_factory {
public:
  void create_accepted( const Socket& sock )
  {
    Conn_type* c = new Conn_type( sock );
    post_create( c );
    c->post_accept();
  }

  virtual void post_create( Conn_type* ) {}
};

} // namespace iqnet

#endif
