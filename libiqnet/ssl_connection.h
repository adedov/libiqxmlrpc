#ifndef _libiqnet_ssl_connection_
#define _libiqnet_ssl_connection_

#include <openssl/ssl.h>
#include <libiqnet/connection.h>
#include <libiqnet/ssl_lib.h>
#include <libiqnet/conn_fabric.h>


namespace iqnet
{
  namespace ssl 
  {
    class Connection;
    class Reaction_connection;
    class Serial_conn_fabric;
  }
};
  
//! Server-side established TCP-connection 
//! with Secure Socket Layer built under it.
class iqnet::ssl::Connection: public iqnet::Connection {
protected:
  ssl::Ctx* ssl_ctx;
  SSL *ssl;
  
public:
  Connection( int sock, const iqnet::Inet_addr&, ssl::Ctx* = 0 );
  ~Connection();
  
  int send( const char*, int );
  int recv( char*, int );

  //! Does ssl_accept()
  void post_accept() { ssl_accept(); }
  //! Does  ssl_connect()
  void post_connect() { ssl_connect(); }

protected:
  //! Performs SSL accepting
  virtual void ssl_accept();
  //! Performs SSL connecting
  virtual void ssl_connect();
};


class iqnet::ssl::Reaction_connection: public ssl::Connection {
  Reactor* reactor;

  enum State { EMPTY, ACCEPTING, READING, WRITING };
  State state;
  
  char* recv_buf;
  const char* send_buf;
  int buf_len;

public:
  Reaction_connection( int, const iqnet::Inet_addr&, ssl::Ctx*, Reactor* = 0 );

  //! A trick for supporting generic fabric.
  void set_reactor( Reactor* r )
  {
    reactor = r;
  }
  
  void post_accept();
  //void post_connect();
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


template <class SSL_conn_type>
class iqnet::ssl::Serial_conn_fabric: public iqnet::Accepted_conn_fabric {
  ssl::Ctx* ctx;
public:
  Serial_conn_fabric( ssl::Ctx* ctx_ ): ctx(ctx_) {}
    
  void create_accepted( int sock, const Inet_addr& peer_addr )
  {
    SSL_conn_type* c = new SSL_conn_type( sock, peer_addr, ctx );
    post_create( c );
    c->post_accept();
  }
  
  virtual void post_create( SSL_conn_type* ) {}
};

#endif
