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
//  $Id: client.h,v 1.10 2004-07-23 08:46:38 adedov Exp $

#ifndef _iqxmlrpc_client_h_
#define _iqxmlrpc_client_h_

#include <memory>
#include "connection.h"
#include "connector.h"
#include "request.h"
#include "response.h"
#include "http.h"

namespace iqxmlrpc 
{
  class Client_connection;
  class Client_base;
  template <class Transport> class Client;

  class Client_timeout;
};


//! Transport independent base class for XML-RPC client's connection.
class iqxmlrpc::Client_connection {
  http::Packet_reader<http::Response_header> preader;

protected:
  unsigned read_buf_sz;
  char *read_buf;
  int timeout;

public:
  Client_connection();
  virtual ~Client_connection();

  Response process_session( 
    const Request&, 
    const std::string& uri,
    const std::string& vhost
  );

  void set_timeout( int seconds )
  {
    timeout = seconds;
  }

protected:
  http::Packet* read_response( const std::string& );
  virtual http::Packet* do_process_session( const std::string& ) = 0;
};


//! Abstract base class for XML-RPC client.
class iqxmlrpc::Client_base {
public:
  virtual ~Client_base() {}

  //! Pure abstract execute() method
  virtual Response execute( const std::string&, const Param_list& ) = 0;
};


//! Template for XML-RPC client class.
template < class Transport >
class iqxmlrpc::Client: public iqxmlrpc::Client_base {
  iqnet::Inet_addr addr;
  std::string uri;
  std::string vhost;
  iqnet::Connector<Transport> ctr;
  int timeout;
  bool non_blocking_flag;

public:
  //! Construct the client
  /*! \param addr_ Actual server address;
      \param uri_  Requested URI (default "/RPC");
      \param host_ Requested virtual host (by default calculated form addr).
  */
  Client( 
    const iqnet::Inet_addr& addr_, 
    const std::string& uri_= "/RPC",
    const std::string& host_ = "" 
  ):
    addr(addr_), 
    uri(uri_),
    vhost(host_.empty() ? addr_.get_host_name() : host_),
    ctr(addr),
    timeout(-1)
  {
  }
  
  //! Set timeout for silence on network in seconds.
  /*! \param seconds TO value in seconds, negative number means infinity.
      \note It is not summary timeout.
      \note Timeout gives no effect on connection process.
  */
  void set_timeout( int seconds )
  {
    if( (timeout = seconds) > 0 )
      non_blocking_flag = true;
  }
  
  //! Perform Remote Procedure Call
  Response execute( const std::string& method_name, const Param_list& pl );
  
  //! Perform Remote Procedure Call with only one parameter transfered
  Response execute( const std::string& method, const Value& val );
};


template <class T>
iqxmlrpc::Response iqxmlrpc::Client<T>::execute( 
  const std::string& method, const Param_list& pl )
{
  Request req( method, pl );
  std::auto_ptr<T> conn( ctr.connect(non_blocking_flag) );
  conn->set_timeout( timeout );
  return conn->process_session( req, uri, vhost );
}


template <class T>
iqxmlrpc::Response iqxmlrpc::Client<T>::execute( 
  const std::string& method, const Value& val )
{
  Param_list pl;
  pl.push_back( val );
  return execute( method, pl );
}


//! Exception which be thrown by client when timeout occured.
class iqxmlrpc::Client_timeout: public iqxmlrpc::Exception {
public:
  Client_timeout():
    Exception( "Broken connection." ) {}
};

#endif
