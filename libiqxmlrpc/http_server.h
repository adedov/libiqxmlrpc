//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqxmlrpc_http_server_h_
#define _libiqxmlrpc_http_server_h_

#include "inet_addr.h"
#include "server.h"

namespace iqxmlrpc {

//! XML-RPC server that works over plain HTTP connections.
class LIBIQXMLRPC_API Http_server: public Server {
public:
  Http_server(const iqnet::Inet_addr& bind_addr, Executor_factory_base*);
};

} // namespace iqxmlrpc

#endif
