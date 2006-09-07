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
//  $Id: executor.h,v 1.15 2006-09-07 04:45:21 adedov Exp $

#ifndef _iqxmlrpc_executor_h_
#define _iqxmlrpc_executor_h_

#include <vector>
#include <deque>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include "api_export.h"
#include "lock.h"
#include "method.h"
#include "reactor_interrupter.h"

namespace iqnet
{
  class Reactor_base;
}

namespace iqxmlrpc
{
class Server;
class Server_connection;
class Response;

class Serial_executor_factory;
class Pool_executor_factory;

struct Serial_executor_traits
{
  typedef Serial_executor_factory Executor_factory;
  typedef iqnet::Null_lock Lock;
};

struct Pool_executor_traits
{
  typedef Pool_executor_factory Executor_factory;
  typedef boost::mutex Lock;
};

//! Abstract executor class. Defines the policy for method execution.
class LIBIQXMLRPC_API Executor {
protected:
  Method* method;
  Interceptor* interceptors;

private:
  Server* server;
  Server_connection* conn;

public:
  Executor( Method*, Server*, Server_connection* );
  virtual ~Executor();

  void set_interceptors(Interceptor* ic) { interceptors = ic; }

  //! Start method execution.
  virtual void execute( const Param_list& params ) = 0;

protected:
  void schedule_response( const Response& );
};


//! Abstract base for Executor's factories.
class LIBIQXMLRPC_API Executor_factory_base {
public:
  virtual ~Executor_factory_base() {}

  virtual Executor* create(
    Method*,
    Server*,
    Server_connection*
  ) = 0;

  virtual iqnet::Reactor_base* create_reactor() = 0;
};


//! Single thread executor.
class LIBIQXMLRPC_API Serial_executor: public Executor {
public:
  Serial_executor( Method* m, Server* s, Server_connection* c ):
    Executor( m, s, c ) {}

  void execute( const Param_list& );
};


//! Factory class for Serial_executor.
class LIBIQXMLRPC_API Serial_executor_factory: public Executor_factory_base {
public:
  Executor* create( Method* m, Server* s, Server_connection* c );
  iqnet::Reactor_base* create_reactor();
};


//! An Executor which plans request to be executed by a pool of threads.
class LIBIQXMLRPC_API Pool_executor: public Executor {
  static iqnet::Reactor_interrupter* reactor_interrupter;

  Pool_executor_factory* pool;
  Param_list params;

public:
  Pool_executor( Pool_executor_factory*, Method*, Server*, Server_connection* );
  ~Pool_executor();

  void execute( const Param_list& );
  void process_actual_execution();
};


//! Factory for Pool_executor objects. It is also serves as a pool of threads.
class LIBIQXMLRPC_API Pool_executor_factory: public Executor_factory_base {
  class Pool_thread;
  friend class Pool_thread;

  boost::thread_group       threads;
  std::vector<Pool_thread*> pool;

  // Objects Pool_thread works with
  std::deque<Pool_executor*> req_queue;
  boost::mutex               req_queue_lock;
  boost::condition           req_queue_cond;

public:
  Pool_executor_factory(unsigned num_threads);
  ~Pool_executor_factory();

  Executor* create( Method* m, Server* s, Server_connection* c );
  iqnet::Reactor_base* create_reactor();

  //! Add some threads to the pool.
  void add_threads(unsigned num);

  void register_executor( Pool_executor* );
};

} // namespace iqxmlrpc

#endif
