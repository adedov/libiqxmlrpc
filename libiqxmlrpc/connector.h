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

#include "socket.h"

#include <string>

namespace iqnet
{

//! Connector template.
template <class Conn_type>
class Connector {
  Inet_addr peer_addr;

public:
  Connector( const iqnet::Inet_addr& peer ):
    peer_addr(peer) {}

  //! Process connection.
  /*! Usage example:
      \code
        class My_conn: public iqnet::Connection {...};
        ...
        iqnet::Inet_addr addr( "xyz.com", 1234 );
        iqnet::Connector<My_conn> ctr( addr );
        My_conn* conn = ctr.connect();
      \endcode
  */
  Conn_type* connect( bool non_block_flag )
  {
    Socket sock;
    sock.connect( peer_addr );
    Conn_type* c = new Conn_type( sock, non_block_flag );
    c->post_connect();
    return c;
  }

  void set_addr( const iqnet::Inet_addr& addr )
  {
    peer_addr = addr;
  }

  const Inet_addr& get_addr() const
  {
    return peer_addr;
  }
};

} // namespace iqnet

#endif
// vim:ts=2:sw=2:et
