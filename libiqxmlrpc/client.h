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
//  $Id: client.h,v 1.1 2004-04-14 08:44:03 adedov Exp $

#ifndef _iqxmlrpc_client_h_
#define _iqxmlrpc_client_h_

#include <memory>
#include "libiqnet/connection.h"
#include "libiqnet/connector.h"
#include "request.h"
#include "response.h"
#include "http.h"

namespace iqxmlrpc 
{
  class Client_connection;
  class Client_base;
  class Client;
};


//! Transport independent base class for XML-RPC client's connection.
class iqxmlrpc::Client_connection: public iqnet::Connection {
  http::Packet_reader<http::Response_header> preader;

protected:
  unsigned read_buf_sz;
  char *read_buf;

public:
  Client_connection( int sock, const iqnet::Inet_addr& peer );
  ~Client_connection();

  Response process_session( 
    const Request&, 
    const std::string& uri, 
    const std::string& lhost 
  );

protected:
  http::Packet* read_response( const std::string& );
  virtual http::Packet* do_process_session( const std::string& ) = 0;
};


//! Abstract base class for XML-RPC client.
class iqxmlrpc::Client_base {
public:
  virtual ~Client_base() {}
  virtual Response execute( const std::string&, const Param_list& ) = 0;
};


//! Template for XML-RPC client class.
template < class Transport >
class iqxmlrpc::Client: public iqxmlrpc::Client_base {
  iqnet::Inet_addr addr;
  std::string local_host;
  std::string uri;
  iqnet::Connector<Transport> ctr;
  
public:
  Client( const iqnet::Inet_addr& addr_, const std::string& uri_="/RPC" ):
    addr(addr_), 
    local_host(iqnet::get_host_name()),
    uri(uri_),
    ctr(addr)
  {
  }
  
  //! Perform Remote Procedure Call.
  Response execute( const std::string& method_name, const Param_list& pl );
};


template <class T>
iqxmlrpc::Response iqxmlrpc::Client<T>::execute( 
  const std::string& method, const Param_list& pl )
{
  Request req( method, pl );
  std::auto_ptr<T> conn( ctr.connect() );
  return conn->process_session( req, uri, local_host );
}

#endif
