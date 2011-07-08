//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_http_auth_plugin_
#define _iqxmlrpc_http_auth_plugin_

#include "sysinc.h"

#include <string>

namespace iqxmlrpc {

//! HTTP Authentication plugin.
class Auth_Plugin_base {
public:
  virtual ~Auth_Plugin_base() {}

  bool authenticate(
    const std::string& user,
    const std::string& password) const;

  bool authenticate_anonymous() const;

private:
  //! User should implement this function. Method must return true when
  //! authentication succeed. Authorization goes synchronously.
  virtual bool do_authenticate(const std::string&, const std::string&) const = 0;

  //! User should implement this function too.
  //! Just return false to make anonymous requests forbiden or
  //! return true to allow clients process requests without authentication.
  virtual bool do_authenticate_anonymous() const = 0;
};

} // namespace iqxmlrpc

#endif
// vim:sw=2:ts=2:et:
