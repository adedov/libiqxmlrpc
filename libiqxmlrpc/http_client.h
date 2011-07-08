//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqxmlrpc_http_client_h_
#define _libiqxmlrpc_http_client_h_

#include "client.h"
#include "client_conn.h"
#include "connector.h"
#include "reactor.h"

namespace iqxmlrpc
{

class Http_proxy_client_connection;

//! XML-RPC \b HTTP client's connection (works in blocking mode).
class LIBIQXMLRPC_API Http_client_connection:
  public iqxmlrpc::Client_connection,
  public iqnet::Connection
{
  std::auto_ptr<iqnet::Reactor_base> reactor;
  std::string out_str;
  http::Packet* resp_packet;

public:
  typedef Http_proxy_client_connection Proxy_connection;

  Http_client_connection( const iqnet::Socket&, bool non_block );

  void handle_input( bool& );
  void handle_output( bool& );

protected:
  http::Packet* do_process_session( const std::string& );
};

//! XML-RPC \b HTTP PROXY client connection.
//! DO NOT USE IT IN YOUR CODE.
class LIBIQXMLRPC_API Http_proxy_client_connection:
  public Http_client_connection
{
public:
  Http_proxy_client_connection( const iqnet::Socket& s, bool non_block ):
    Http_client_connection( s, non_block ) {}

private:
  virtual std::string decorate_uri() const;
};

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
