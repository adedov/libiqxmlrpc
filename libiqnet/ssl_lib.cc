#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h> 
#include "ssl_lib.h"


SSL_CTX* SSL_lib::ctx = 0;


void SSL_lib::init( const std::string& cert_path, const std::string& key_path )
{
  SSL_load_error_strings();
  SSL_library_init();
  
  SSL_METHOD *meth = SSLv2_server_method();
  ctx  = SSL_CTX_new( meth );
  
  if( !ctx )
    throw error();
      
  if( SSL_CTX_use_certificate_file(ctx, cert_path.c_str(), SSL_FILETYPE_PEM) <= 0 ) 
    throw error();
    
  if( SSL_CTX_use_PrivateKey_file(ctx, key_path.c_str(), SSL_FILETYPE_PEM) <= 0 )
    throw error();
      
  if( !SSL_CTX_check_private_key(ctx) ) 
    throw error();
}


// --------- SSL::error -----------
SSL_lib::error::error():
  std::runtime_error( ERR_error_string( SSL_get_error( 0, 0 ), 0 ) ) // ??
{
}


SSL_lib::error::error( int ssl_err ):
  std::runtime_error( ERR_error_string( ssl_err, 0 ) ) // not thread-safe!
{
}


SSL_lib::error::error( SSL* ssl, int ret ):
  std::runtime_error( ERR_error_string( SSL_get_error(ssl, ret), 0 ) ) // not thread-safe!
{
}
