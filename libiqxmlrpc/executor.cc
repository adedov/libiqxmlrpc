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
//  $Id: executor.cc,v 1.1 2004-04-14 08:44:03 adedov Exp $

#include "executor.h"
#include "except.h"
#include "response.h"
#include "server.h"

using namespace iqxmlrpc;


Executor::Executor( Method* m, Server* s, Server_connection* cb ):
  method(m),
  server(s),
  conn(cb)
{
}


Executor::~Executor()
{
  delete method;
}


void Executor::schedule_response( const Response& resp )
{
  server->schedule_response( resp, conn, this );
}


// ----------------------------------------------------------------------------
void Serial_executor::execute( const Param_list& params )
{
  std::string error_msg;
  int fault_code;
  
  try 
  {
    Value result(0);
    method->execute( params, result );
    schedule_response( Response(result) );
    return;
  }
  catch( const xmlpp::exception& e )
  {
    error_msg  = e.what();
    fault_code = Fault_code::xml_parser;
  }
  catch( const Exception& e )
  {
    error_msg = e.what();
    fault_code = e.code();
  }

  // Should be invoked in exception case only.
  schedule_response( Response(fault_code, error_msg) );
}
