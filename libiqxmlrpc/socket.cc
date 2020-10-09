//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <errno.h>
#include <boost/cerrno.hpp>
#include "socket.h"
#include "net_except.h"

#if _MSC_VER >= 1700
#include <ws2tcpip.h>
#endif

using namespace iqnet;

Socket::Socket()
{
  if( (sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1 )
    throw network_error( "Socket::Socket" );

#ifndef _WIN32
  {
  int enable = 1;
  setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable) );
  }
#endif //_WIN32

#if defined(__APPLE__)
  {
  int enable = 1;
  setsockopt( sock, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(enable) );
  }
#endif
}

Socket::Socket( Socket::Handler h, const Inet_addr& addr ):
  sock(h),
  peer(addr)
{
}

void Socket::shutdown()
{
  ::shutdown( sock, 2 );
}

void Socket::close()
{
#ifdef _WIN32
  closesocket(sock);
#else
  ::close( sock );
#endif //_WIN32
}

void Socket::set_non_blocking( bool flag )
{
#ifdef _WIN32
  unsigned long f = flag ? 1 : 0;
  if( ioctlsocket(sock, FIONBIO, &f) != 0 )
    throw network_error( "Socket::set_non_blocking");
#else
  if( !flag )
    return;

  if( fcntl( sock, F_SETFL, O_NDELAY ) == -1 )
    throw network_error( "Socket::set_non_blocking" );
#endif //_WIN32
}

#if defined(MSG_NOSIGNAL)
#define IQXMLRPC_NOPIPE MSG_NOSIGNAL
#else
#define IQXMLRPC_NOPIPE 0
#endif

size_t Socket::send( const char* data, size_t len )
{
  int ret = ::send( sock, data, static_cast<int>(len), IQXMLRPC_NOPIPE);

  if( ret == -1 )
    throw network_error( "Socket::send" );

  return static_cast<size_t>(ret);
}

size_t Socket::recv( char* buf, size_t len )
{
  int ret = ::recv( sock, buf, static_cast<int>(len), 0 );

  if( ret == -1 )
    throw network_error( "Socket::recv" );

  return static_cast<size_t>(ret);
}

void Socket::send_shutdown( const char* data, size_t len )
{
  send(data, len);
  const struct linger ling = {1, 0};
  ::setsockopt( sock, SOL_SOCKET, SO_LINGER, reinterpret_cast<const char*>(&ling), sizeof(ling) );
  ::shutdown( sock, 1 );
}

void Socket::bind( const Inet_addr& addr )
{
  const sockaddr* saddr = reinterpret_cast<const sockaddr*>(addr.get_sockaddr());

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
  sockaddr_in addr;
  socklen_t len = sizeof(sockaddr_in);

  Handler new_sock = ::accept( sock, reinterpret_cast<sockaddr*>(&addr), &len );
  if( new_sock == -1 )
    throw network_error( "Socket::accept" );

  return Socket( new_sock, Inet_addr(addr) );
}

bool Socket::connect( const iqnet::Inet_addr& peer_addr )
{
  const sockaddr* saddr = reinterpret_cast<const sockaddr*>(peer_addr.get_sockaddr());

  int code = ::connect(sock, saddr, sizeof(sockaddr_in));
  bool wouldblock = false;

  if( code == -1 ) {
#ifndef _WIN32
    wouldblock = errno == EINPROGRESS;
#else
    wouldblock = get_last_error() == WSAEWOULDBLOCK;
#endif

    if (!wouldblock)
      throw network_error( "Socket::connect" );
  }

  peer = peer_addr;
  return !wouldblock;
}

Inet_addr Socket::get_addr() const
{
  sockaddr_in saddr;
  socklen_t saddr_len = sizeof(saddr);

  if (::getsockname(sock, reinterpret_cast<sockaddr*>(&saddr), &saddr_len) == -1)
    throw network_error( "Socket::get_addr" );

  return Inet_addr(reinterpret_cast<const sockaddr_in&>(saddr));
}

int Socket::get_last_error()
{
  int err = 0;
#ifndef _WIN32
  socklen_t int_sz = sizeof(err);
  ::getsockopt( sock, SOL_SOCKET, SO_ERROR, &err, &int_sz );
#else
  err=WSAGetLastError();
#endif
  return err;
}
