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
//  $Id: connection.cc,v 1.7 2006-09-07 09:35:41 adedov Exp $

#include "connection.h"
#include "net_except.h"

using namespace iqnet;


Connection::Connection( const Socket& s ):
  sock(s)
{
}


Connection::~Connection()
{
  ::shutdown( sock.get_handler(), 2 );
  sock.close();
}


void Connection::finish()
{
  sock.shutdown();
  sock.close();
}


void Connection::handle_error( bool& )
{
  int sock_err = sock.get_last_error();

  if( sock_err )
  {
    errno = sock_err;
    throw network_error( "Connection::throw_sock_exception" );
  }
}


int Connection::send( const char* data, int len )
{
  return sock.send( data, len );
}


int Connection::recv( char* buf, int len )
{
  return sock.recv( buf, len );
}
