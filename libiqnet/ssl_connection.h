#ifndef _libiqnet_ssl_connection_
#define _libiqnet_ssl_connection_

#include <openssl/ssl.h>
#include <libiqnet/connection.h>
#include <libiqnet/ssl_lib.h>


namespace iqnet
{

namespace ssl 
{
  class Connection;
  class Reaction_connection;
}

  
//! Server-side established TCP-connection 
//! with Secure Socket Layer built under it.
class ssl::Connection: public iqnet::Connection {
protected:
  ssl::Ctx* ssl_ctx;
  SSL *ssl;
  
public:
  Connection( int sock, const iqnet::Inet_addr&, ssl::Ctx* );
  ~Connection();
  
  int send( const char*, int );
  int recv( char*, int );

  //! Alias for ssl_accept()
  void post_init() { ssl_accept(); }

protected:
  //! Perform SSL accepting (for server side only).
  virtual void ssl_accept();
};


class ssl::Reaction_connection: public ssl::Connection {
  Reactor* reactor;

  enum State { EMPTY, ACCEPTING, READING, WRITING };
  State state;
  
  char* recv_buf;
  const char* send_buf;
  int buf_len;

public:
  Reaction_connection( int, const iqnet::Inet_addr&, ssl::Ctx*, Reactor* );

  void post_init();
  void handle_input( bool& );
  void handle_output( bool& );

private:
  void try_send();
  int  try_recv();

protected:
  void ssl_accept();
  void reg_send( const char*, int );
  void reg_recv( char*, int );

  virtual void accept_succeed() = 0;
  virtual void recv_succeed( bool& terminate, int req_len, int real_len ) = 0;
  virtual void send_succeed( bool& terminate ) = 0;
};

};

#endif
