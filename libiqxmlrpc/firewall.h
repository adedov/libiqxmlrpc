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
//  $Id: acceptor.h,v 1.8 2006-09-07 04:45:21 adedov Exp $

#ifndef _libiqxmlrpc_firewall_h_
#define _libiqxmlrpc_firewall_h_

namespace iqnet {

//! Firewall base class.
/*! Used by Acceptor to find out whether it should
    accept XML-RPC requests from specific IP.
*/
class LIBIQXMLRPC_API Firewall_base {
public:
  virtual ~Firewall_base() {}

  //! Must return bool to grant client to send request.
  virtual bool grant( const iqnet::Inet_addr& ) = 0;

  //! Override this method for custom good-bye message.
  /*! Return empty string for closing connection silently. */
  virtual std::string message()
  {
    return "HTTP/1.0 403 Forbidden\r\n";
  }
};

} // namespace iqnet

#endif
