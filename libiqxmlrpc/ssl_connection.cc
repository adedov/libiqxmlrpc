//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <openssl/err.h>
#include "ssl_connection.h"

using namespace iqnet;

ssl::Connection::Connection( const Socket& s ):
  iqnet::Connection( s ),
  ssl_ctx( ssl::ctx )
{
  if( !ssl_ctx )
    throw ssl::not_initialized();

  ssl = SSL_new( ssl_ctx->context() );

  if( !ssl )
    throw ssl::exception();

  if( !SSL_set_fd( ssl, static_cast<int>(sock.get_handler()) ) )
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
  ssl_ctx->prepare_verify(ssl, true);
  int ret = SSL_accept( ssl );

  if( ret != 1 )
    throw_io_exception( ssl, ret );
}


void ssl::Connection::ssl_connect()
{
  ssl_ctx->prepare_verify(ssl, false);
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


size_t ssl::Connection::send( const char* data, size_t len )
{
  int ret = SSL_write( ssl, data, static_cast<int>(len) );

  if( static_cast<size_t>(ret) != len )
    throw_io_exception( ssl, ret );

  return static_cast<size_t>(ret);
}


size_t ssl::Connection::recv( char* buf, size_t len )
{
  int ret = SSL_read( ssl, buf, static_cast<int>(len) );

  if( ret <= 0 )
    throw_io_exception( ssl, ret );

  return static_cast<size_t>(ret);
}


inline bool ssl::Connection::shutdown_recved()
{
  return (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) != 0;
}


inline bool ssl::Connection::shutdown_sent()
{
  return SSL_get_shutdown(ssl) & SSL_SENT_SHUTDOWN;
}


// ----------------------------------------------------------------------------
ssl::Reaction_connection::Reaction_connection( const Socket& s, Reactor_base* r ):
  ssl::Connection( s ),
  reactor(r)
{
  sock.set_non_blocking( true );
}


void ssl::Reaction_connection::post_accept()
{
  reg_accept();
}


void ssl::Reaction_connection::post_connect()
{
  reg_connect();
}


void ssl::Reaction_connection::ssl_accept()
{
  ssl::Connection::ssl_accept();
  state = EMPTY;
}


void ssl::Reaction_connection::ssl_connect()
{
  ssl::Connection::ssl_connect();
  state = EMPTY;
}


void ssl::Reaction_connection::switch_state( bool& terminate )
{
  try
  {
    switch( state )
    {
      case ACCEPTING:
        ssl_accept();
        accept_succeed();
        break;

      case CONNECTING:
        ssl_connect();
        connect_succeed();
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
        terminate = true;
    }
  }
  catch( const ssl::need_read& )
  {
//    std::cout << "need_read" << std::endl;
    reactor->register_handler( this, Reactor_base::INPUT );
  }
  catch( const ssl::need_write& )
  {
//    std::cout << "need_write" << std::endl;
    reactor->register_handler( this, Reactor_base::OUTPUT );
  }
  catch( const ssl::connection_close& )
  {
//    std::cout << "connection_close " << e.is_clean() << std::endl;
    reg_shutdown();
  }
}


void ssl::Reaction_connection::handle_input( bool& terminate )
{
  reactor->unregister_handler( this, Reactor_base::INPUT );
  switch_state( terminate );
}


void ssl::Reaction_connection::handle_output( bool& terminate )
{
  reactor->unregister_handler( this, Reactor_base::OUTPUT );
  switch_state( terminate );
}


void ssl::Reaction_connection::try_send()
{
  send( send_buf, buf_len );
  state = EMPTY;
}


size_t ssl::Reaction_connection::try_recv()
{
  size_t ln = recv( recv_buf, buf_len );
  state = EMPTY;

  return ln;
}


bool ssl::Reaction_connection::reg_shutdown()
{
  state = SHUTDOWN;

  if( !shutdown_sent() )
  {
    reactor->register_handler( this, Reactor_base::OUTPUT );
  }
  else if( !shutdown_recved() )
  {
    reactor->register_handler( this, Reactor_base::INPUT );
  }
  else
  {
    state = EMPTY;
    return true;
  }

  return false;
}


void ssl::Reaction_connection::reg_accept()
{
  state = ACCEPTING;
  reactor->register_handler( this, Reactor_base::INPUT );
}


void ssl::Reaction_connection::reg_connect()
{
  state = CONNECTING;
  reactor->register_handler( this, Reactor_base::OUTPUT );
}


void ssl::Reaction_connection::reg_send( const char* buf, size_t len )
{
  state = WRITING;
  send_buf = buf;
  buf_len = len;
  reactor->register_handler( this, Reactor_base::OUTPUT );
}


void ssl::Reaction_connection::reg_recv( char* buf, size_t len )
{
  state = READING;
  recv_buf = buf;
  buf_len = len;
  reactor->register_handler( this, Reactor_base::INPUT );

  if( SSL_pending(ssl) )
    reactor->fake_event( this, Reactor_base::INPUT );
}
