#include <iostream>
#include <openssl/err.h>
#include <libiqnet/ssl_connection.h>

using namespace iqnet;


ssl::Connection::Connection( int s, const iqnet::Inet_addr& addr, ssl::Ctx* c ):
  iqnet::Connection( s, addr ),
  ssl_ctx( c ? c : ssl::ctx )
{
  if( !ssl_ctx )
    throw ssl::not_initialized();
  
  ssl = SSL_new( ssl_ctx->context() );
  
  if( !ssl )
    throw ssl::exception();
    
  if( !SSL_set_fd( ssl, sock ) )
    throw ssl::exception();
}


ssl::Connection::~Connection()
{
  SSL_shutdown( ssl );
  SSL_free( ssl );
}


void ssl::Connection::ssl_accept()
{
  int ret = SSL_accept( ssl );
  
  if( ret != 1 )
    throw_io_exception( ssl, ret );
}


void ssl::Connection::ssl_connect()
{
  int ret = SSL_connect( ssl );
  
  if( ret != 1 )
    throw_io_exception( ssl, ret );
}


int ssl::Connection::send( const char* data, int len )
{
  int ret = SSL_write( ssl, data, len );
  
  if( ret != len )
    throw_io_exception( ssl, ret );

  return ret;
}


int ssl::Connection::recv( char* buf, int len )
{
  int ret = SSL_read( ssl, buf, len );

  if( ret < 0 )
    throw_io_exception( ssl, ret );

  return ret;
}


// ----------------------------------------------------------------------------
ssl::Reaction_connection::Reaction_connection
  ( int sock, const iqnet::Inet_addr& addr, ssl::Ctx* ctx, Reactor* r ):
  ssl::Connection( sock, addr, ctx ),
  reactor(r)
{
}


void ssl::Reaction_connection::post_accept()
{
  try {
    set_non_blocking( true );
    ssl_accept();
    accept_succeed();
  }
  catch( const ssl::need_read& )
  {
    reactor->register_handler( this, Reactor::INPUT );
  }
  catch( const ssl::need_write& )
  {
    reactor->register_handler( this, Reactor::OUTPUT );
  }
}


void ssl::Reaction_connection::ssl_accept()
{
  state = ACCEPTING;
  Connection::ssl_accept();
  state = EMPTY;
}


void ssl::Reaction_connection::handle_input( bool& terminate )
{
  reactor->unregister_handler( this, Reactor::INPUT );
  
  try {
    switch( state )
    {
      case ACCEPTING:
        ssl_accept();
        accept_succeed();
        break;
      
      case READING:
        recv_succeed( terminate, buf_len, try_recv() );
        break;
      
      case WRITING:
        try_send();
        send_succeed( terminate );
        break;
      
      case EMPTY:
      default:
        recv_succeed( terminate, 0, 0 );
        break;
    }
  }
  catch( const ssl::need_read& )
  {
    std::cout << "hi: need_read" << std::endl;   
    reactor->register_handler( this, Reactor::INPUT );
  }
  catch( const ssl::need_write& )
  {
    std::cout << "hi: need_write" << std::endl;    
    reactor->register_handler( this, Reactor::OUTPUT );
  }  
}


void ssl::Reaction_connection::handle_output( bool& terminate )
{
  reactor->unregister_handler( this, Reactor::OUTPUT );
  
  try {
    switch( state )
    {
      case ACCEPTING:
        ssl_accept();
        accept_succeed();
        break;
      
      case READING:
        recv_succeed( terminate, buf_len, try_recv() );
        break;
      
      case WRITING:
        try_send();
        send_succeed( terminate );
        break;
      
      case EMPTY:
      default:
        send_succeed( terminate );
        break;
    }
  }
  catch( const ssl::need_read& )
  {
    std::cout << "ho: need_read" << std::endl;
    reactor->register_handler( this, Reactor::INPUT );
  }
  catch( const ssl::need_write& )
  {
    std::cout << "ho: need_write" << std::endl;
    reactor->register_handler( this, Reactor::OUTPUT );
  }  
}


void ssl::Reaction_connection::try_send()
{
  state = WRITING;
  send( send_buf, buf_len );
  state = EMPTY;
}


int ssl::Reaction_connection::try_recv()
{
  state = READING;
  int ln = recv( recv_buf, buf_len );
  state = EMPTY;
  
  return ln;
}


void ssl::Reaction_connection::reg_send( const char* buf, int len )
{
  state = WRITING;
  send_buf = buf;
  buf_len = len;
  reactor->register_handler( this, Reactor::OUTPUT );
}


void ssl::Reaction_connection::reg_recv( char* buf, int len )
{
  state = READING;
  recv_buf = buf;
  buf_len = len;
  reactor->register_handler( this, Reactor::INPUT );
}
