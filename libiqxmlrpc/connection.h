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
//  $Id: connection.h,v 1.5 2005-03-23 18:26:00 bada Exp $

#ifndef _libiqnet_connection_h_
#define _libiqnet_connection_h_

#include <string>
#include "inet_addr.h"
#include "reactor.h"
#include "net_except.h"


namespace iqnet 
{
  class Connection;
}


//! An established TCP-connection.
/*!
    A build block for connection handling. 
    Have to be created by connection's factory.
*/
class iqnet::Connection: public iqnet::Event_handler {
protected:
  Socket sock;

public:
  Connection( const Socket& );
  virtual ~Connection();

  void finish();
  void handle_error( bool& );

  virtual void post_accept() {}
  virtual void post_connect() {}
  
  const iqnet::Inet_addr& get_peer_addr() const 
  { 
    return sock.get_peer_addr(); 
  }
  
  Socket::Handler get_handler() const
  {
    return sock.get_handler();
  }
  
  virtual int send( const char*, int );
  virtual int recv( char*, int );
};


#endif
