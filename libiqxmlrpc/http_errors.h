//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqxmlrpc_http_errors_h_
#define _libiqxmlrpc_http_errors_h_

#include "http.h"

namespace iqxmlrpc {
namespace http {

//! HTTP/1.1 400 Bad request
class LIBIQXMLRPC_API Bad_request: public Error_response {
public:
  Bad_request():
    Error_response( "Bad request", 400 ) {}
};

//! HTTP/1.1 401 Unauthorized
class LIBIQXMLRPC_API Unauthorized: public Error_response {
public:
  Unauthorized():
    Error_response( "Unauthorized", 401 )
  {
    header_->set_option( "www-authenticate", "Basic realm=\"\"" );
  }
};

//! HTTP/1.1 405 Method not allowed
class LIBIQXMLRPC_API Method_not_allowed: public Error_response {
public:
  Method_not_allowed():
    Error_response( "Method not allowed", 405 )
  {
    header_->set_option( "allowed:", "POST" );
  }
};

//! HTTP/1.1 411 Length Required
class LIBIQXMLRPC_API Length_required: public Error_response {
public:
  Length_required():
    Error_response( "Content-Length Required", 411 ) {}
};

//! HTTP/1.1 413 Request Entity Too Large
class LIBIQXMLRPC_API Request_too_large: public Error_response {
public:
  Request_too_large():
    Error_response( "Request Entity Too Large", 413 ) {}
};

//! HTTP/1.1 415 Unsupported media type
class LIBIQXMLRPC_API Unsupported_content_type: public Error_response {
public:
  Unsupported_content_type(const std::string& wrong):
    Error_response( "Unsupported media type '" + wrong + "'", 415 ) {}
};

} // namespace http
} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
