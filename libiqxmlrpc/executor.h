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
//  $Id: executor.h,v 1.2 2004-04-22 09:25:56 adedov Exp $

#ifndef _iqxmlrpc_executor_h_
#define _iqxmlrpc_executor_h_

#include "lock.h"
#include "method.h"

namespace iqxmlrpc 
{
  class Server;
  class Server_connection;
  class Response;

  class Executor;
  class Serial_executor;

  class Executor_fabric_base;
  template <class Executor_type> class Executor_fabric;
};


//! Abstract executor class. Defines the policy for method execution.
class iqxmlrpc::Executor {
protected:
  iqxmlrpc::Method* method;

private:
  Server* server;
  Server_connection* conn;
  
public:
  Executor( Method*, Server*, Server_connection* );
  virtual ~Executor();

  //! Start method execution.
  virtual void execute( const Param_list& params ) = 0;

protected:
  void schedule_response( const Response& );
};


//! Abstract base for Executor's fabrics.
class iqxmlrpc::Executor_fabric_base {
public:
  virtual Executor* create( 
    Method*, 
    Server*, 
    Server_connection* 
  ) = 0;

  virtual iqnet::Lock* create_lock() = 0;
};


//! Simple template for Executor's fabric.
template < class Executor_type >
class iqxmlrpc::Executor_fabric: public iqxmlrpc::Executor_fabric_base {
public:
  typedef typename Executor_type::Lock Lock;

  //! Create concrete Executor's object.
  Executor* create( Method* m, Server* s, Server_connection* c )
  {
    return new Executor_type( m, s, c );
  }
  
  //! Create appropriate Lock object, which corresponds execution policy.
  iqnet::Lock* create_lock()
  {
    return new Lock;
  }
};


//! An Executor which provides serial execution policy.
class iqxmlrpc::Serial_executor: public iqxmlrpc::Executor {
public:
  typedef iqnet::Null_lock Lock;

  Serial_executor( Method* m, Server* s, Server_connection* c ):
    Executor( m, s, c ) {}

  void execute( const Param_list& params );
};


#endif
