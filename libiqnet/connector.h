//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
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
//  
//  $Id: connector.h,v 1.4 2004-04-14 07:28:07 adedov Exp $

#ifndef _libiqnet_connector_h_
#define _libiqnet_connector_h_

#include <string>
#include "inet_addr.h"


namespace iqnet 
{
  class Connector_base;
  template <class Conn_type> class Connector;
};


//! Connector's base. Does actual socket's connection.
class iqnet::Connector_base {
protected:
  Inet_addr peer_addr;
  
public:
  Connector_base( const iqnet::Inet_addr& addr ): 
    peer_addr(addr) {}
      
  virtual ~Connector_base() {}

protected:
  int socket_connect();
};


//! Connector template.
template <class Conn_type>
class iqnet::Connector: public iqnet::Connector_base {
public:
  Connector( const iqnet::Inet_addr& peer ):
    Connector_base( peer ) {}

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
  Conn_type* connect()
  {
    int sock = socket_connect();
    Conn_type* c = new Conn_type( sock, peer_addr );
    c->post_connect();
    return c;
  }
};


#endif
