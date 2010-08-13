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
//  $Id: acceptor.cc,v 1.11 2006-09-07 09:35:41 adedov Exp $

#include "acceptor.h"

#include "connection.h"
#include "conn_factory.h"
#include "inet_addr.h"
#include "net_except.h"

using namespace iqnet;


Acceptor::Acceptor( int port, Accepted_conn_factory* factory_, Reactor_base* reactor_ ):
  factory(factory_),
  reactor(reactor_),
  firewall(0)
{
  sock.bind( port );
  listen();
  reactor->register_handler( this, Reactor_base::INPUT );
}


Acceptor::~Acceptor()
{
  reactor->unregister_handler(this);
  sock.close();
}


void Acceptor::set_firewall( iqnet::Firewall_base* fw )
{
  delete firewall;
  firewall = fw;
}


void Acceptor::handle_input( bool& )
{
  accept();
}


inline void Acceptor::listen()
{
  sock.listen( 5 );
}


void Acceptor::accept()
{
  Socket new_sock( sock.accept() );

  if( firewall )
  {
    if( !firewall->grant( new_sock.get_peer_addr() ) )
    {
      // Just close socket.
      // So client should receive "connection reset by peer" message.
      new_sock.close();
      return;
    }
  }

  factory->create_accepted( new_sock );
}
