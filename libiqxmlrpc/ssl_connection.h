//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_ssl_connection_
#define _libiqnet_ssl_connection_

#include "connection.h"
#include "conn_factory.h"
#include "reactor.h"
#include "ssl_lib.h"

#include <openssl/ssl.h>

namespace iqnet {
namespace ssl {

//! SSL connection class.
class LIBIQXMLRPC_API Connection: public iqnet::Connection {
protected:
  ssl::Ctx* ssl_ctx;
  SSL *ssl;

public:
  Connection( const Socket& sock );
  ~Connection();

  void shutdown();
  int send( const char*, int );
  int recv( char*, int );

  //! Does ssl_accept()
  void post_accept();
  //! Does ssl_connect()
  void post_connect();

protected:
  //! Performs SSL accepting
  virtual void ssl_accept();
  //! Performs SSL connecting
  virtual void ssl_connect();

  bool shutdown_recved();
  bool shutdown_sent();
};

//! Server-side established SSL-connection based on reactive model
//! (with underlying non-blocking socket).
class LIBIQXMLRPC_API Reaction_connection: public ssl::Connection {
  Reactor_base* reactor;

  enum State { EMPTY, ACCEPTING, CONNECTING, READING, WRITING, SHUTDOWN };
  State state;

  char* recv_buf;
  const char* send_buf;
  int buf_len;

public:
  Reaction_connection( const Socket&, Reactor_base* = 0 );

  //! A trick for supporting generic factory.
  void set_reactor( Reactor_base* r )
  {
    reactor = r;
  }

  void post_accept();
  void post_connect();
  void handle_input( bool& );
  void handle_output( bool& );

private:
  void switch_state( bool& terminate );
  void try_send();
  int  try_recv();

protected:
  void ssl_accept();
  void ssl_connect();
  //! Returns true if shutdown already performed.
  bool reg_shutdown();
  void reg_accept();
  void reg_connect();
  void reg_send( const char*, int );
  void reg_recv( char*, int );

  //! Overwrite it for server connection.
  virtual void accept_succeed()  {};
  //! Overwrite it for client connection.
  virtual void connect_succeed() {};

  virtual void recv_succeed( bool& terminate, int req_len, int real_len ) = 0;
  virtual void send_succeed( bool& terminate ) = 0;
};

} // namespace ssl
} // namespace iqnet

#endif
