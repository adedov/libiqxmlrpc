#include <iostream>
#include <openssl/err.h>
#include <libiqnet/ssl_connection.h>

using namespace iqnet;


ssl::Connection::Connection( int s, const iqnet::Inet_addr& addr ):
  iqnet::Connection( s, addr ),
  ssl_ctx( ssl::ctx )
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
  SSL_free( ssl );
}


void ssl::Connection::post_accept()
{
  ssl_accept();
}


void ssl::Connection::post_connect()
{
  ssl_connect();
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


void ssl::Connection::shutdown()
{
  if( shutdown_recved() && shutdown_sent() )
    return;
  
  int ret = SSL_shutdown( ssl );
  switch( ret )
  {
    case 1:
      return;

    case 0:
      SSL_shutdown( ssl );
      SSL_set_shutdown( ssl, SSL_RECEIVED_SHUTDOWN );
      break;

    default:
      throw_io_exception( ssl, ret );
  }
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


inline bool ssl::Connection::shutdown_recved()
{
  return SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN;
}


inline bool ssl::Connection::shutdown_sent()
{
  return SSL_get_shutdown(ssl) & SSL_SENT_SHUTDOWN;
}


// ----------------------------------------------------------------------------
ssl::Reaction_connection::Reaction_connection
  ( int sock, const iqnet::Inet_addr& addr, Reactor* r ):
  ssl::Connection( sock, addr ),
  reactor(r)
{
  set_non_blocking( true );
}


void ssl::Reaction_connection::post_accept()
{
  try {
    state = ACCEPTING;
    ssl::Connection::post_accept();
    accept_succeed();
  }
  catch( const ssl::need_read& )
  {
    std::cout << "post_accept: need_read" << std::endl;
    reactor->register_handler( this, Reactor::INPUT );
  }
  catch( const ssl::need_write& )
  {
    std::cout << "post_accept: need_write" << std::endl;
    reactor->register_handler( this, Reactor::OUTPUT );
  }
}


void ssl::Reaction_connection::ssl_accept()
{
  ssl::Connection::ssl_accept();
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
      
      case SHUTDOWN:
        ssl::Connection::shutdown();
        terminate = true;
        break;
          
      case EMPTY:
      default:
        std::cout << "hi: EMPTY" << std::endl;
        terminate = true;
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
  catch( const ssl::connection_close& e )
  {
    std::cout << "hi: connection_close " << e.is_clean() << std::endl;
    reg_shutdown();
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

      case SHUTDOWN:
        ssl::Connection::shutdown();
        terminate = true;
        break;
          
      case EMPTY:
      default:
        std::cout << "hi: EMPTY" << std::endl;
        terminate = true;
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
  catch( const ssl::connection_close& e )
  {
    std::cout << "hi: connection_close " << e.is_clean() << std::endl;
    reg_shutdown();
  }
}


void ssl::Reaction_connection::try_send()
{
  send( send_buf, buf_len );
  state = EMPTY;
}


int ssl::Reaction_connection::try_recv()
{
  int ln = recv( recv_buf, buf_len );
  state = EMPTY;
  
  return ln;
}


bool ssl::Reaction_connection::reg_shutdown()
{
  state = SHUTDOWN;

  if( !shutdown_sent() )
  {
    reactor->register_handler( this, Reactor::OUTPUT );
  }
  else if( !shutdown_recved() )
  {
    reactor->register_handler( this, Reactor::INPUT );
  }
  else
  {
    state = EMPTY;
    return true;
  }
  
  return false;
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
  
  if( SSL_pending(ssl) )
    reactor->fake_event( this, Reactor::INPUT );
}
