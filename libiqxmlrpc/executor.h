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
//  $Id: executor.h,v 1.5 2004-06-07 09:44:59 adedov Exp $

#ifndef _iqxmlrpc_executor_h_
#define _iqxmlrpc_executor_h_

#include <vector>
#include <deque>
#include "lock.h"
#include "method.h"
#include "sigsock.h"
#include "thread.h"


namespace iqxmlrpc 
{
  class Server;
  class Server_connection;
  class Response;

  class Executor;
  class Serial_executor;
  class Pool_executor;

  class Executor_fabric_base;
  class Serial_executor_fabric;
  class Pool_executor_fabric;
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
  virtual ~Executor_fabric_base() {}

  virtual Executor* create( 
    Method*, 
    Server*, 
    Server_connection* 
  ) = 0;

  virtual iqnet::Lock* create_lock() = 0;
};


//! Single thread executor.
class iqxmlrpc::Serial_executor: public iqxmlrpc::Executor {
public:
  typedef iqnet::Null_lock Lock;

  Serial_executor( Method* m, Server* s, Server_connection* c ):
    Executor( m, s, c ) {}

  void execute( const Param_list& params );
};


//! Factory class for Serial_executor.
class iqxmlrpc::Serial_executor_fabric: public iqxmlrpc::Executor_fabric_base {
public:
  Executor* create( Method* m, Server* s, Server_connection* c )
  {
    return new Serial_executor( m, s, c );
  }
  
  iqnet::Lock* create_lock()
  {
    return new Serial_executor::Lock;
  }
};


//! An Executor which plans request to be executed by a pool of threads.
class iqxmlrpc::Pool_executor: public iqxmlrpc::Executor {
  static iqnet::Alarm_socket* alarm_sock;

  Pool_executor_fabric* pool;
  Param_list params;

public:
  typedef iqnet::Mutex_lock Lock;

  Pool_executor( Pool_executor_fabric*, Method*, Server*, Server_connection* );
  ~Pool_executor();

  void execute( const Param_list& params );
  void process_actual_execution();
};


//! Fabric for Pool_executor objects. It is also serves as a pool of threads.
class iqxmlrpc::Pool_executor_fabric: public iqxmlrpc::Executor_fabric_base {
  class Pool_thread;
  friend class Pool_thread;

  std::vector<Pool_thread*> pool;
  std::deque<Pool_executor*> req_queue;
  iqnet::Cond req_queue_cond;
  
public:
  Pool_executor_fabric( unsigned pool_size );
  ~Pool_executor_fabric();

  Executor* create( Method* m, Server* s, Server_connection* c )
  {
    return new Pool_executor( this, m, s, c );
  }
  
  iqnet::Lock* create_lock()
  {
    return new Pool_executor::Lock;
  }

  void register_executor( Pool_executor* );
};


#endif
