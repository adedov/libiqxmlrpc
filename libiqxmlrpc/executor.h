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
//  $Id: executor.h,v 1.7 2005-07-19 15:45:46 bada Exp $

#ifndef _iqxmlrpc_executor_h_
#define _iqxmlrpc_executor_h_

#include <vector>
#include <deque>
#include <boost/thread.hpp>
#include "lock.h"
#include "method.h"
#include "sigsock.h"
#include "mt_synch.h"

namespace iqxmlrpc 
{
  class Server;
  class Server_connection;
  class Response;

  class Executor;
  class Serial_executor;
  class Pool_executor;

  class Executor_factory_base;
  class Serial_executor_factory;
  class Pool_executor_factory;
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


//! Abstract base for Executor's factories.
class iqxmlrpc::Executor_factory_base {
public:
  virtual ~Executor_factory_base() {}

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
class iqxmlrpc::Serial_executor_factory: public iqxmlrpc::Executor_factory_base {
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

  Pool_executor_factory* pool;
  Param_list params;

public:
  typedef iqnet::Mutex_lock Lock;

  Pool_executor( Pool_executor_factory*, Method*, Server*, Server_connection* );
  ~Pool_executor();

  void execute( const Param_list& params );
  void process_actual_execution();
};


//! Factory for Pool_executor objects. It is also serves as a pool of threads.
class iqxmlrpc::Pool_executor_factory: public iqxmlrpc::Executor_factory_base {
  class Pool_thread;
  friend class Pool_thread;

  boost::thread_group threads;
  std::vector<Pool_thread*> pool;

  // Objects Pool_thread works with
  std::deque<Pool_executor*> req_queue;
  iqnet::Cond req_queue_cond;
  
public:
  Pool_executor_factory( unsigned pool_size );
  ~Pool_executor_factory();

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
