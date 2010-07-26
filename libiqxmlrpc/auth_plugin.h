//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2007 Anton Dedov
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA

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
