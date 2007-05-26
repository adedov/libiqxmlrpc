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

#ifndef _iqxmlrpc_client_opts_h_
#define _iqxmlrpc_client_opts_h_

#include <string>
#include "inet_addr.h"

namespace iqxmlrpc {

class LIBIQXMLRPC_API Client_options {
public:
  Client_options(
    const iqnet::Inet_addr& addr,
    const std::string& uri,
    const std::string& vhost
  ):
    addr_(addr),
    uri_(uri),
    vhost_(vhost.empty() ? addr.get_host_name() : vhost),
    keep_alive_(false),
    timeout_(-1),
    non_blocking_flag_(false)
  {
  }

  const iqnet::Inet_addr&  addr()         const { return addr_; }
  const std::string&       uri()          const { return uri_; }
  const std::string&       vhost()        const { return vhost_; }
  int                      timeout()      const { return timeout_; }
  bool                     non_blocking() const { return non_blocking_flag_; }
  bool                     keep_alive()   const { return keep_alive_; }

  void set_timeout( int seconds )
  {
    if( (timeout_ = seconds) > 0 )
      non_blocking_flag_ = true;
  }

  void set_keep_alive( bool keep_alive )
  {
    keep_alive_ = keep_alive;
  }

private:
  iqnet::Inet_addr addr_;
  std::string      uri_;
  std::string      vhost_;
  bool             keep_alive_;
  int              timeout_;
  bool             non_blocking_flag_;
};

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
