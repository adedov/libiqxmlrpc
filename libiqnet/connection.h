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
//  $Id: connection.h,v 1.6 2004-04-14 07:28:07 adedov Exp $

#ifndef _libiqnet_connection_h_
#define _libiqnet_connection_h_

#include <string>
#include "inet_addr.h"
#include "reactor.h"
#include "net_except.h"


namespace iqnet 
{
  class Connection;
}


//! An established TCP-connection.
/*!
    A build block for connection handling. 
    Have to be created by Connection_fabric.
*/
class iqnet::Connection: public iqnet::Event_handler {
protected:
  int sock;
  iqnet::Inet_addr peer_addr;
  std::string cache;

public:
  Connection( int socket_d, const iqnet::Inet_addr& peer_addr );
  virtual ~Connection();

  virtual void post_accept() {}
  virtual void post_connect() {}
  
  const iqnet::Inet_addr& get_peer_addr() const { return peer_addr; }
  
  /*! \b Can \b not cause SIGPIPE signal. */
  virtual int send( const char*, int );
  /*! \b Can \b not cause SIGPIPE signal. */
  virtual int recv( char*, int );

  void send_str( const std::string& );
  const std::string recv_str();  

protected:
  int get_fd() const { return sock; }

  void set_non_blocking( bool );
  bool get_non_blocking() const;
};


#endif
