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
//  $Id: connector.cc,v 1.3 2004-04-19 08:39:53 adedov Exp $

#include "sysinc.h"
#include "connector.h"
#include "net_except.h"

using namespace iqnet;


int Connector_base::socket_connect()
{
  int sock = socket( PF_INET, SOCK_STREAM, 0 );
  if( sock == -1 )
    throw network_error( "socket" );

  int enable = 1;
  setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable) );

  const sockaddr* saddr = 
    reinterpret_cast<const sockaddr*>(peer_addr.get_sockaddr());
  
  if( ::connect(sock, saddr, sizeof(sockaddr_in)) )
    throw network_error( "connect" );
  
  return sock;
}
