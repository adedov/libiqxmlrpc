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
//  $Id: null_transport.h,v 1.2 2004-03-29 06:23:18 adedov Exp $

#ifndef _libiqxmlrpc_null_transport_h_
#define _libiqxmlrpc_null_transport_h_

#include <string>
#include <libiqxmlrpc/request.h>
#include <libiqxmlrpc/response.h>


namespace iqxmlrpc 
{
  class Method_dispatcher;
  class Server;
  class Client;
};


//! Transport independent XML-RPC server.
class iqxmlrpc::Server {
  Method_dispatcher* disp;
  Request* request;
  Value resp_value;

public:
  Server( Method_dispatcher* );
  virtual ~Server();

  Response execute( const std::string& xmlrpc_request );

private:
  void parse_request( const std::string& );
  void execute();
};


//! Abstract class for XML-RPC client.
/*! Inherit it to create concrete transport specific ones. */
class iqxmlrpc::Client {
public:
  virtual ~Client() {}

  Response execute( const std::string& method_name, const Param_list& pl );
    
protected:
  //! Overwrite it to perform actual Remote Procedure Call.
  virtual std::string do_execute( const Request& ) = 0;
};


#endif
