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
//  $Id: acceptor.cc,v 1.1 2004-04-22 09:25:56 adedov Exp $

#include <iostream>
#include "sysinc.h"
#include "acceptor.h"
#include "connection.h"
#include "conn_fabric.h"
#include "net_except.h"
#include "inet_addr.h"

using namespace iqnet;


Acceptor::Acceptor( int port, Accepted_conn_fabric* fabric_, Reactor* reactor_ ):
  fabric(fabric_),
  reactor(reactor_)
{
  if( (sock = socket( PF_INET, SOCK_STREAM, 0 )) == -1 )
    throw network_error( "socket" );

  int enable = 1;
  setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable) );
  
  Inet_addr addr( port );
  const sockaddr* saddr = reinterpret_cast<const sockaddr*>(addr.get_sockaddr());
  if( bind( sock, saddr, sizeof(sockaddr_in) ) == -1 )
    throw network_error( "bind" );
  
  listen();
  reactor_->register_handler( this, Reactor::INPUT );
}


Acceptor::~Acceptor()
{
  close( sock );
}


void Acceptor::handle_input( bool& )
{
  accept();
}


inline void Acceptor::listen()
{
  if( ::listen( sock, 5 ) == -1 )
    throw network_error( "listen" );
}


void Acceptor::accept()
{
  struct sockaddr_in addr;
  socklen_t len;
  
  int new_sock = ::accept( sock, reinterpret_cast<sockaddr*>(&addr), &len );  
  if( new_sock == -1 )
    throw network_error( "accept" );

  fabric->create_accepted( new_sock, Inet_addr( addr ) );
  listen();
}


const Inet_addr Acceptor::get_addr_listening() const
{
  struct sockaddr_in addr;
  socklen_t len;
  
  getsockname( sock, reinterpret_cast<sockaddr*>(&addr), &len );
  return Inet_addr( addr );
}
