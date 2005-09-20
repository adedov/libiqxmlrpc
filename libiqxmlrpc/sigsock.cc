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
//  $Id: sigsock.cc,v 1.3 2005-09-20 16:03:00 bada Exp $

#include "sigsock.h"
#include "sysinc.h"
#include "net_except.h"

using namespace iqnet;

#ifndef AF_LOCAL
#define AF_LOCAL AF_UNIX
#endif

Alarm_socket::Alarm_socket( Reactor_base* r ):
  reactor(r)
{
  socketpair( AF_LOCAL, SOCK_STREAM, 0, sock );
  int mode = fcntl( sock[0], F_GETFL, 0 );
  
  if( fcntl( sock[0], F_SETFL, mode | O_NDELAY ) == -1 )
    throw network_error( "Alarm_socket: fcntl( F_SETFL )" );

  r->register_handler( this, Reactor_base::INPUT );
}


Alarm_socket::~Alarm_socket()
{
  close( sock[0] );
  close( sock[1] );
}


void Alarm_socket::handle_input( bool& )
{
  char buf;
  ::read( sock[0], &buf, 1 );
}


void Alarm_socket::send_alarm()
{
  boost::mutex::scoped_lock lk(lock);
  ::write( sock[1], "\0", 1 );
}
