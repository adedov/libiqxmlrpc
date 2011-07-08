//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "auth_plugin.h"

namespace iqxmlrpc {

bool Auth_Plugin_base::authenticate(
  const std::string& user,
  const std::string& password) const
{
  return do_authenticate(user, password);
}

bool Auth_Plugin_base::authenticate_anonymous() const
{
  return do_authenticate_anonymous();
}

} // namespace iqxmlrpc

// vim:sw=2:ts=2:et:
