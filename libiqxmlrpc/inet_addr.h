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
//  $Id: inet_addr.h,v 1.2 2004-04-27 07:18:03 adedov Exp $

#ifndef _libiqnet_inet_addr_h_
#define _libiqnet_inet_addr_h_

#include <string>
#include "sysinc.h"

//! Object-oriented networking/multithreading infrastructure.
namespace iqnet 
{
  class Inet_addr;
  
  //! Returns host.domain of local processor.
  std::string get_host_name();
};


//! An object representation of internet address.
/*! A wrapper for sockaddr_in system structure. */
class iqnet::Inet_addr {
  struct sockaddr_in sa;
  std::string host;
  int port;
  
public:
  Inet_addr( const struct sockaddr_in& );
  Inet_addr( const std::string& host, int port );
  Inet_addr( int port );

  virtual ~Inet_addr() {}
  
  const struct sockaddr_in* get_sockaddr() const { return &sa; }
  const std::string& get_host_name() const { return host; }
  int get_port() const { return port; }
};

#endif
