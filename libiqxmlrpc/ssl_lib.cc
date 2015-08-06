//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifdef WIN32
#include <winsock2.h>
#endif

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

#include <sstream>
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

Ctx* ctx = 0;
boost::once_flag ssl_init;
int iqxmlrpc_ssl_data_idx = 0;

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

  iqxmlrpc_ssl_data_idx = SSL_get_ex_new_index(0, (void*)"iqxmlrpc verifier", NULL, NULL, NULL);
}

//
// Ctx
//

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

namespace {

void
set_common_options(SSL_CTX* ctx)
{
  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
}

int
iqxmlrpc_SSL_verify(int prev_ok, X509_STORE_CTX* ctx)
{
  SSL* ssl = reinterpret_cast<SSL*>(X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx()));
  ConnectionVerifier* v = reinterpret_cast<ConnectionVerifier*>(SSL_get_ex_data(ssl, iqxmlrpc_ssl_data_idx));
  return v->verify(prev_ok, ctx);
}

} // anonymous namespace

//
// ConnectionVerifier
//

ConnectionVerifier::~ConnectionVerifier()
{
}

int
ConnectionVerifier::verify(bool prev_ok, X509_STORE_CTX* ctx) const
{
  try {
    return do_verify(prev_ok, ctx);
  } catch (...) {
    // TODO: log ability?
    return 0;
  }
}

std::string
ConnectionVerifier::cert_finger_sha256(X509_STORE_CTX* ctx) const
{
  X509* x = X509_STORE_CTX_get_current_cert(ctx);
  const EVP_MD* digest = EVP_get_digestbyname("sha256");
  unsigned int n = 0;
  unsigned char md[EVP_MAX_MD_SIZE];
  X509_digest(x, digest, md, &n);

  std::ostringstream ss;
  for(int i = 0; i < 32; i++)
     ss << std::hex << int(md[i]);

  return ss.str();
}

//
// Ctx
//

struct Ctx::Impl {
  SSL_CTX* ctx;
  ConnectionVerifier* server_verifier;
  ConnectionVerifier* client_verifier;
  bool require_client_cert;

  Impl():
    server_verifier(0),
    client_verifier(0),
    require_client_cert(false)
  {
  }
};

Ctx::Ctx( const std::string& cert_path, const std::string& key_path, bool client ):
  impl_(new Impl)
{
  boost::call_once(ssl_init, init_library);
  impl_->ctx = SSL_CTX_new( client ? SSLv23_method() : SSLv23_server_method() );
  set_common_options(impl_->ctx);

  if(
    !SSL_CTX_use_certificate_file( impl_->ctx, cert_path.c_str(), SSL_FILETYPE_PEM ) ||
    !SSL_CTX_use_PrivateKey_file( impl_->ctx, key_path.c_str(), SSL_FILETYPE_PEM ) ||
    !SSL_CTX_check_private_key( impl_->ctx )
  )
    throw exception();
}


Ctx::Ctx():
  impl_(new Impl)
{
  boost::call_once(ssl_init, init_library);
  impl_->ctx = SSL_CTX_new( SSLv23_client_method() );
  set_common_options( impl_->ctx );
}


Ctx::~Ctx()
{
}

SSL_CTX*
Ctx::context()
{
  return impl_->ctx;
}

void
Ctx::verify_server(ConnectionVerifier* v)
{
  impl_->server_verifier = v;
}

void
Ctx::verify_client(bool require_certificate, ConnectionVerifier* v)
{
  impl_->require_client_cert = require_certificate;
  impl_->client_verifier = v;
}

void
Ctx::prepare_verify(SSL* ssl, bool server)
{
  ConnectionVerifier* v = server ? impl_->client_verifier : impl_->server_verifier;
  int mode = v ? SSL_VERIFY_PEER : SSL_VERIFY_NONE;

  if (server && impl_->require_client_cert)
    mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;

  if (v) {
    SSL_set_verify(ssl, mode, iqxmlrpc_SSL_verify);
    SSL_set_ex_data(ssl, iqxmlrpc_ssl_data_idx, (void*)v);
  } else {
    SSL_set_verify(ssl, mode, 0);
  }
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
      bool clean = (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) != 0;
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
