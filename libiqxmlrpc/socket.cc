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
//  $Id: socket.cc,v 1.5 2004-08-17 03:52:29 adedov Exp $

#include <iostream>
#include "sysinc.h"
#include "socket.h"
#include "net_except.h"

using namespace iqnet;


Socket::Socket()
{
  if( (sock = socket( PF_INET, SOCK_STREAM, 0 )) == -1 )
    throw network_error( "Socket::Socket" );

#ifndef _WINDOWS
  int enable = 1;
  setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable) );
  
  struct linger ling;
  ling.l_onoff = 0;
  setsockopt( sock, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling) );
#endif //_WINDOWS
}


Socket::Socket( Socket::Handler h, const Inet_addr& addr ):
  sock(h),
  peer(addr)
{
}


void Socket::close()
{
#ifdef _WINDOWS
	closesocket(sock);
#else
  ::close( sock );
#endif //_WINDOWS
}


void Socket::set_non_blocking( bool flag )
{
#ifdef _WINDOWS
	unsigned long f = flag ? 1 : 0;
	if( ioctlsocket(sock, FIONBIO, &f) != 0 )
		throw network_error( "Socket::set_non_blocking");
#else
  if( !flag )
    return;

  if( fcntl( sock, F_SETFL, O_NDELAY ) == -1 )
    throw network_error( "Socket::set_non_blocking" );
#endif //_WINDOWS
}


#ifndef MSG_NOSIGNAL
#ifndef WINDOWS_
// MSG_NOSIGNAL tells send() and recv() not to generate SIGPIPE when
// the other side closes the connection.  This is a nice feature, but
// unfortunately not portable.
#warning "Allowing Broken Pipe signals (SIGPIPE) due to lack of MSG_NOSIGNAL."
#warning "If you get unwanted Broken Pipe signals, consider ignoring them:"
#warning "signal(SIGPIPE, SIG_IGN);"
#endif // WINDOWS_
#define MSG_NOSIGNAL 0
#endif

int Socket::send( const char* data, int len )
{
#ifdef _WINDOWS
  int ret = ::send( sock, data, len, 0);
#else
  int ret = ::send( sock, data, len, MSG_NOSIGNAL );
#endif //_WINDOWS
  
  if( ret == -1 )
    throw network_error( "Socket::send" );

  return ret;
}


int Socket::recv( char* buf, int len )
{
#ifdef _WINDOWS
  int ret = ::recv( sock, buf, len, 0 );
#else
  int ret = ::recv( sock, buf, len, MSG_NOSIGNAL );
#endif //_WINDOWS
  
  if( ret == -1 )
    throw network_error( "Socket::recv" );

  return ret;
}


void Socket::bind( int port )
{
  Inet_addr addr( port );
  const sockaddr* saddr = 
    reinterpret_cast<const sockaddr*>(addr.get_sockaddr());

  if( ::bind( sock, saddr, sizeof(sockaddr_in) ) == -1 )
    throw network_error( "Socket::bind" );
}


void Socket::listen( unsigned blog )
{
  if( ::listen( sock, blog ) == -1 )
    throw network_error( "Socket::listen" );
}


Socket Socket::accept()
{
  struct sockaddr_in addr;
  socklen_t len;
  
  Handler new_sock = ::accept( sock, reinterpret_cast<sockaddr*>(&addr), &len );  
  if( new_sock == -1 )
    throw network_error( "Socket::accept" );

  return Socket( new_sock, Inet_addr(addr) );
}


void Socket::connect( const iqnet::Inet_addr& peer_addr )
{
  const sockaddr* saddr = 
    reinterpret_cast<const sockaddr*>(peer_addr.get_sockaddr());
  
  if( ::connect(sock, saddr, sizeof(sockaddr_in)) )
    throw network_error( "Socket::connect" );
  
  peer = peer_addr;
}


int Socket::get_last_error()
{
  int err = 0;
  socklen_t int_sz = 0;
  
  ::getsockopt( sock, SOL_SOCKET, SO_ERROR, &err, &int_sz );
  return err;
}
