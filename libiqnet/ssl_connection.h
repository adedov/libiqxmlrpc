#ifndef _libiqnet_ssl_connection_
#define _libiqnet_ssl_connection_

#include <openssl/ssl.h>
#include <libiqnet/connection.h>

namespace iqnet 
{
  class SSL_Connection;
};


//! Server-side established TCP-connection 
//! with Secure Socket Layer built under it.
class iqnet::SSL_Connection: public iqnet::Connection {
  SSL_CTX* ssl_ctx;
  SSL *ssl;
  
public:
  SSL_Connection( int sock, const iqnet::Inet_addr&, SSL_CTX* );
  ~SSL_Connection();
  
  int send( const char*, int );
  int recv( char*, int );

  //! Alias for ssl_accept()
  void post_init() { ssl_accept(); }

  //! Perform SSL accepting (for server side only).
  void ssl_accept();
};

#endif
