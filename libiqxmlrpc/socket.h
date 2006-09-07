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
//  $Id: socket.h,v 1.8 2006-09-07 04:45:21 adedov Exp $

#ifndef _libiqnet_socket_h_
#define _libiqnet_socket_h_

#include "api_export.h"
#include "inet_addr.h"

namespace iqnet
{

//! Relatively portable socket class.
class LIBIQXMLRPC_API Socket {
public:
#ifdef _WINDOWS
	typedef SOCKET Handler;
#else
  typedef int Handler;
#endif //_WINDOWS

private:
  Handler sock;
  Inet_addr peer;

public:
  //! Creates TCP, reusable socket.
  Socket();
  //! Create object from existing socket handler.
  Socket( Handler, const Inet_addr& );
  //! Destructor. Does not close actual socket.
  virtual ~Socket() {}

  Handler get_handler() const { return sock; }

  void shutdown();
  void close();

  //! \note Does not disable non-blocking mode under UNIX.
  void set_non_blocking( bool );

  /*! \b Can \b not cause SIGPIPE signal. */
  virtual int send( const char*, int );
  /*! \b Can \b not cause SIGPIPE signal. */
  virtual int recv( char*, int );

  void   bind( int port );
  void   bind( const std::string& host, int port );
  void   listen( unsigned backlog = 5 );
  Socket accept();
  void   connect( const iqnet::Inet_addr& );

  //! Returns an inet addr the socket asscociated with.
  Inet_addr get_addr() const;

  //! Returns peer addr of connected or accepted socket.
  const Inet_addr& get_peer_addr() const { return peer; }

  //! Returns last error occured with socket.
  int get_last_error();
};

} // namespace iqnet

#endif
