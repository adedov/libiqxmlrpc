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
//  $Id: http_server.h,v 1.3 2004-07-20 05:45:28 adedov Exp $

#ifndef _libiqxmlrpc_http_server_h_
#define _libiqxmlrpc_http_server_h_

#include "connector.h"
#include "server.h"

namespace iqxmlrpc
{
  class Http_server_connection;
};


//! Represents server-side \b HTTP non-blocking connection.
class iqxmlrpc::Http_server_connection: 
  public iqnet::Connection,
  public iqxmlrpc::Server_connection 
{
  iqnet::Reactor* reactor;

public:
  Http_server_connection( const iqnet::Socket& );

  void set_reactor( iqnet::Reactor* r ) { reactor = r; }

  void post_accept();  
  void finish();
  
  void handle_input( bool& );
  void handle_output( bool& );

  void schedule_response( http::Packet* packet );
  
  bool catch_in_reactor() const { return true; }
  void log_exception( const std::exception& );
  void log_unknown_exception();  
};


#endif
