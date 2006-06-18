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
//  $Id: inet_addr.cc,v 1.2 2006-06-18 14:08:26 bada Exp $

#include "sysinc.h"
#include "inet_addr.h"
#include "net_except.h"

using namespace iqnet;


std::string iqnet::get_host_name()
{
  char buf[256];
  buf[255] = 0;
  ::gethostname( buf, 255 );
 
  return buf;
}


Inet_addr::Inet_addr( const std::string& host_, int port_ ):
  host(host_), port(port_)
{
  struct hostent *hent = gethostbyname( host.c_str() );
  if( !hent )
    throw network_error( "gethostbyname" );
  
  sa.sin_family = PF_INET;
  sa.sin_port = htons(port);
  memcpy( (char*)&sa.sin_addr, (char*)hent->h_addr, hent->h_length );
}


Inet_addr::Inet_addr( int port_ ):
  host("127.0.0.1"), port(port_)
{
  sa.sin_family = PF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = INADDR_ANY;
}


Inet_addr::Inet_addr( const struct sockaddr_in& sa_ ):
  sa(sa_)
{
  host = inet_ntoa( sa.sin_addr );
  port = ntohs( sa.sin_port );
}
