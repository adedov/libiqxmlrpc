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
//  $Id: acceptor.h,v 1.4 2004-04-14 07:28:07 adedov Exp $

#ifndef _libiqnet_acceptor_h_
#define _libiqnet_acceptor_h_

#include <string>
#include "inet_addr.h"
#include "reactor.h"

namespace iqnet 
{
  class Accepted_conn_fabric;
  class Connection;
  class Acceptor;
};

//! An implementation of pattern that separates TCP-connection
//! establishment from connection handling.
/*!
    Acceptor uses creates server-side socket on port 'port' 
    and waits for incoming connections. When incoming connection 
    is occured the Acceptor is using instance of Connection_fabric 
    to create a specific connection handler.
*/
class iqnet::Acceptor: public iqnet::Event_handler {
  int sock;
  Accepted_conn_fabric *fabric;
  Reactor *reactor;
    
public:
  Acceptor( int port, Accepted_conn_fabric*, Reactor* );
  virtual ~Acceptor();

  const iqnet::Inet_addr get_addr_listening() const;
  void handle_input( bool& );

protected:
  void finish() {}
  int get_fd() const { return sock; }
  
  void accept();
  void listen();
};

#endif
