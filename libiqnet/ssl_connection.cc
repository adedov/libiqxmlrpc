#include <iostream>
#include <openssl/err.h>
#include "ssl_connection.h"
#include "ssl_lib.h"

using namespace iqnet;


SSL_Connection::SSL_Connection( 
    int s, const iqnet::Inet_addr& addr, SSL_CTX* ssl_ctx_ 
  ):
    Connection( s, addr ),
    ssl_ctx( ssl_ctx_ )
{
  ssl = SSL_new( ssl_ctx );
  
  if( !ssl )
    throw SSL_lib::error();
    
  if( !SSL_set_fd( ssl, sock ) )
    throw SSL_lib::error( ssl, 0 );
}


SSL_Connection::~SSL_Connection()
{
  SSL_free( ssl );
}


void SSL_Connection::ssl_accept()
{
  int ret = SSL_accept( ssl );
  
  if( ret != 1 )
    throw SSL_lib::error( ssl, ret );
}


int SSL_Connection::send( const char* data, int len )
{
  int ret = SSL_write( ssl, data, len );
  
  if( ret != len )
    throw SSL_lib::error( ssl, ret );

  return ret;
}


int SSL_Connection::recv( char* buf, int len )
{
  int ret = SSL_read( ssl, buf, len );
  
  if( ret < 0 )
    throw SSL_lib::error( ssl, ret );

  return ret;
}
