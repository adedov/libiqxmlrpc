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

#ifndef _libiqnet_connector_h_
#define _libiqnet_connector_h_

#include "client_conn.h"

namespace iqnet {

class LIBIQXMLRPC_API Connector_base {
  Inet_addr peer_addr;

public:
  Connector_base( const iqnet::Inet_addr& peer );
  virtual ~Connector_base();

  //! Process connection.
  iqxmlrpc::Client_connection* connect(int timeout);

private:
  virtual iqxmlrpc::Client_connection*
  create_connection(const Socket&) = 0;
};

template <class Conn_type>
class Connector: public Connector_base {
public:
  Connector( const iqnet::Inet_addr& peer ):
    Connector_base(peer)
  {
  }

private:
  virtual iqxmlrpc::Client_connection*
  create_connection(const Socket& s)
  {
    Conn_type* c = new Conn_type( s, true );
    c->post_connect();
    return c;
  }
};

} // namespace iqnet

#endif
// vim:ts=2:sw=2:et
