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
//  $Id: conn_fabric.h,v 1.4 2004-03-29 06:20:15 adedov Exp $

#ifndef _libiqnet_connection_fabric_
#define _libiqnet_connection_fabric_

#include <string>
#include <libiqnet/inet_addr.h>

namespace iqnet 
{
  class Accepted_conn_fabric;
    
  template<class Conn_type> 
  class Serial_conn_fabric;
};


//! Abstract fabric for accepted connections.
class iqnet::Accepted_conn_fabric {
public:
  virtual ~Accepted_conn_fabric() {}
  virtual void create_accepted( int sock, const Inet_addr& peer_addr ) = 0;
};


//! Fabric for single threaded connections.
template <class Conn_type>
class iqnet::Serial_conn_fabric: public iqnet::Accepted_conn_fabric {
public:
  void create_accepted( int sock, const Inet_addr& peer_addr )
  {
    Conn_type* c = new Conn_type( sock, peer_addr );
    post_create( c );
    c->post_accept();
  }
  
  virtual void post_create( Conn_type* ) {}
};

#endif
