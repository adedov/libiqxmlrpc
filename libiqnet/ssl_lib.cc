#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h> 
#include <libiqnet/net_except.h>
#include <libiqnet/ssl_lib.h>

using namespace iqnet::ssl;

iqnet::ssl::Ctx* iqnet::ssl::ctx = 0;
bool Ctx::initialized = false;


Ctx* Ctx::server_ctx( const std::string& cert_path, const std::string& key_path )
{
  return new Ctx( cert_path, key_path );
}


Ctx* Ctx::client_ctx()
{
  return new Ctx;
}


Ctx::Ctx( const std::string& cert_path, const std::string& key_path )
{
  init_library();
  
  SSL_METHOD *meth = SSLv23_server_method();
  ctx = SSL_CTX_new( meth );
  
  if( 
    !SSL_CTX_use_certificate_file( ctx, cert_path.c_str(), SSL_FILETYPE_PEM ) ||
    !SSL_CTX_use_PrivateKey_file( ctx, key_path.c_str(), SSL_FILETYPE_PEM ) ||
    !SSL_CTX_check_private_key( ctx ) 
  )
    throw exception();
}


Ctx::Ctx()
{
  init_library();
  
  SSL_METHOD *meth = SSLv23_client_method();
  ctx = SSL_CTX_new( meth );
}


Ctx::~Ctx()
{
}


void Ctx::init_library()
{
  if( !Ctx::initialized )
  {
    SSL_load_error_strings();
    SSL_library_init();
    Ctx::initialized = true;
  }
}


// ----------------------------------------------------------------------------
exception::exception() throw():
  ssl_err( ERR_get_error() ),
  msg( ERR_reason_error_string(ssl_err) )
{
}


exception::exception( unsigned long err ) throw():
  ssl_err(err),
  msg( ERR_reason_error_string(ssl_err) )
{
}


exception::exception( const std::string& msg_ ) throw():
  ssl_err(0),
  msg( msg_ )
{
}


// ----------------------------------------------------------------------------
void iqnet::ssl::throw_io_exception( SSL* ssl, int ret )
{
  int code = SSL_get_error( ssl, ret );
  switch( code )
  {
    case SSL_ERROR_NONE:
      return;
    
    case SSL_ERROR_WANT_READ:
      throw need_read();
    
    case SSL_ERROR_WANT_WRITE:
      throw need_write();
    
    case SSL_ERROR_SYSCALL:
      throw iqnet::network_error( "iqnet::ssl::throw_io_exception" );
    
    case SSL_ERROR_SSL:
      throw exception();
    
    default:
      throw io_error( code );
  }
}
