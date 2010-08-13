//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2006 Anton Dedov
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
//  $Id: acceptor.h,v 1.8 2006-09-07 04:45:21 adedov Exp $

#ifndef _libiqnet_acceptor_h_
#define _libiqnet_acceptor_h_

#include "inet_addr.h"
#include "reactor.h"
#include "socket.h"

namespace iqnet {

class Accepted_conn_factory;

//! Firewall base class.
/*! Used by Acceptor to find out whether it should
    accept XML-RPC requests from specific IP.
*/
class LIBIQXMLRPC_API Firewall_base {
public:
  virtual ~Firewall_base() {}

  //! Must return bool to grant client to send request.
  virtual bool grant( const iqnet::Inet_addr& ) = 0;
};


//! An implementation of pattern that separates TCP-connection
//! establishment from connection handling.
/*!
    Acceptor uses creates server-side socket on port 'port'
    and waits for incoming connections. When incoming connection
    is occured the Acceptor is using instance of Connection_factory
    to create a specific connection handler.
*/
class LIBIQXMLRPC_API Acceptor: public Event_handler {
  Socket sock;
  Accepted_conn_factory *factory;
  Reactor_base *reactor;
  Firewall_base* firewall;

public:
  Acceptor( int port, Accepted_conn_factory*, Reactor_base* );
  virtual ~Acceptor();

  void set_firewall( iqnet::Firewall_base* );

  void handle_input( bool& );

protected:
  void finish() {}
  Socket::Handler get_handler() const { return sock.get_handler(); }

  void accept();
  void listen();
};

} // namespace iqnet

#endif
