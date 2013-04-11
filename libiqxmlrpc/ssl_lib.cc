//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>

#ifndef WIN32
#include <pthread.h>
#endif

#include "ssl_lib.h"
#include "net_except.h"

namespace iqnet {
namespace ssl {

//
// Mutli-threading support stuff
//

class LockContainer: boost::noncopyable {
public:
  LockContainer():
    size(CRYPTO_num_locks()),
    locks(new boost::mutex[size])
  {
  }

  ~LockContainer();

  size_t size;
  boost::mutex* locks;
};

void
openssl_lock_callback(int mode, int n, const char* /*file*/, int /*line*/)
{
  static LockContainer lks;
  // assert n < lks.size

  boost::mutex& m = lks.locks[n];
  if (mode & CRYPTO_LOCK) {
    m.lock();
  } else {
    m.unlock();
  }
}

#ifndef _WIN32
unsigned long
openssl_id_function()
{
  return (unsigned long)(pthread_self());
}
#endif

LockContainer::~LockContainer()
{
   if (CRYPTO_get_locking_callback() == &openssl_lock_callback)
    CRYPTO_set_locking_callback(0);

#ifndef _WIN32
  if (CRYPTO_get_id_callback() == &openssl_id_function)
    CRYPTO_set_id_callback(0);
#endif

  // do not try to unlock locks
  delete[] locks;
}

void
init_library()
{
  SSL_load_error_strings();
  SSL_library_init();

  if (!CRYPTO_get_locking_callback())
    CRYPTO_set_locking_callback(&openssl_lock_callback);

#ifndef _WIN32
  if (!CRYPTO_get_id_callback())
    CRYPTO_set_id_callback(&openssl_id_function);
#endif
}

Ctx* ctx = 0;
boost::once_flag ssl_init;


Ctx* Ctx::client_server( const std::string& cert_path, const std::string& key_path )
{
  return new Ctx( cert_path, key_path, true );
}


Ctx* Ctx::server_only( const std::string& cert_path, const std::string& key_path )
{
  return new Ctx( cert_path, key_path, false );
}


Ctx* Ctx::client_only()
{
  return new Ctx;
}


Ctx::Ctx( const std::string& cert_path, const std::string& key_path, bool client )
{
  boost::call_once(ssl_init, init_library);
  ctx = SSL_CTX_new( client ? SSLv23_method() : SSLv23_server_method() );
  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

  if(
    !SSL_CTX_use_certificate_file( ctx, cert_path.c_str(), SSL_FILETYPE_PEM ) ||
    !SSL_CTX_use_PrivateKey_file( ctx, key_path.c_str(), SSL_FILETYPE_PEM ) ||
    !SSL_CTX_check_private_key( ctx )
  )
    throw exception();
}


Ctx::Ctx()
{
  boost::call_once(ssl_init, init_library);
  ctx = SSL_CTX_new( SSLv23_client_method() );
  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
}


Ctx::~Ctx()
{
}


// ----------------------------------------------------------------------------
exception::exception() throw():
  ssl_err( ERR_get_error() ),
	  msg( ERR_reason_error_string(ssl_err) )
{
	msg.insert(0, "SSL: ");
}


exception::exception( unsigned long err ) throw():
  ssl_err(err),
  msg( ERR_reason_error_string(ssl_err) )
{
	msg.insert(0, "SSL: ");
}


exception::exception( const std::string& msg_ ) throw():
  ssl_err(0),
  msg( msg_ )
{
	msg.insert(0, "SSL: ");
}


// ----------------------------------------------------------------------------
void throw_io_exception( SSL* ssl, int ret )
{
  int code = SSL_get_error( ssl, ret );
  switch( code )
  {
    case SSL_ERROR_NONE:
      return;

    case SSL_ERROR_ZERO_RETURN:
    {
      bool clean = SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN;
      throw connection_close( clean );
    }

    case SSL_ERROR_WANT_READ:
      throw need_read();

    case SSL_ERROR_WANT_WRITE:
      throw need_write();

    case SSL_ERROR_SYSCALL:
      if( !ret )
        throw connection_close( false );
      else
        throw iqnet::network_error( "iqnet::ssl::throw_io_exception" );

    case SSL_ERROR_SSL:
      throw exception();

    default:
      throw io_error( code );
  }
}

} // namespace ssl
} // namespace iqnet
