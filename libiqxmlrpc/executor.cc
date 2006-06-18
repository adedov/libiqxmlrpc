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
//  $Id: executor.cc,v 1.14 2006-06-18 14:08:26 bada Exp $

#include "executor.h"
#include "except.h"
#include "response.h"
#include "reactor_impl.h"
#include "server.h"
#include "util.h"

using namespace iqxmlrpc;
typedef boost::mutex::scoped_lock scoped_lock;

Executor::Executor( Method* m, Server* s, Server_connection* cb ):
  method(m),
  interceptors(0),
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
  try {
    Value result(0);
    method->process_execution( interceptors, params, result );
    schedule_response( Response(result) );
  }
  catch( const Fault& f )
  {
    schedule_response( Response( f.code(), f.what() ) );
  }
}


Executor* Serial_executor_factory::create(
  Method* m, Server* s, Server_connection* c )
{
  return new Serial_executor( m, s, c );
}


iqnet::Reactor_base* Serial_executor_factory::create_reactor()
{
  return new iqnet::Reactor<iqnet::Null_lock>;
}


// ----------------------------------------------------------------------------
#ifndef DOXYGEN_SHOULD_SKIP_THIS
class Pool_executor_factory::Pool_thread {
  unsigned id;
  Pool_executor_factory* pool;

public:
  Pool_thread( unsigned id_, Pool_executor_factory* pool_ ):
    id(id_),
    pool(pool_) 
  {
  }

  // thread's entry point
  void operator ()();
};
#endif


void Pool_executor_factory::Pool_thread::operator ()()
{
  Pool_executor_factory::Pool_thread* obj = 
    static_cast<Pool_executor_factory::Pool_thread*>(this);
  
  Pool_executor_factory* pool = obj->pool;

  for(;;)
  {
    scoped_lock lk(pool->req_queue_lock);
    
    if (pool->req_queue.empty()) 
    {
      pool->req_queue_cond.wait(lk);

      if (pool->req_queue.empty())
        continue;
    }
    
    Pool_executor* executor = pool->req_queue.front();
    pool->req_queue.pop_front();
    lk.unlock();

    executor->process_actual_execution();
  }
}


// ----------------------------------------------------------------------------
Pool_executor_factory::Pool_executor_factory(unsigned numthreads)
{
  add_threads(numthreads);
}


Pool_executor_factory::~Pool_executor_factory()
{
  util::delete_ptrs(pool.begin(), pool.end());
  scoped_lock lk(req_queue_lock);
  util::delete_ptrs(req_queue.begin(), req_queue.end());
}


Executor* Pool_executor_factory::create(
  Method* m, Server* s, Server_connection* c )
{
  return new Pool_executor( this, m, s, c );
}


iqnet::Reactor_base* Pool_executor_factory::create_reactor()
{
  return new iqnet::Reactor<boost::mutex>;
}


void Pool_executor_factory::add_threads( unsigned num )
{
  for( unsigned i = 0; i < num; ++i )
  {
    Pool_thread* t = new Pool_thread(i, this);
    pool.push_back(t);
    threads.create_thread(*t);
  }
}


void Pool_executor_factory::register_executor( Pool_executor* executor )
{
  scoped_lock lk(req_queue_lock);
  req_queue.push_back(executor);
  req_queue_cond.notify_all();
}


// ----------------------------------------------------------------------------
iqnet::Reactor_interrupter* Pool_executor::reactor_interrupter = 0;


Pool_executor::Pool_executor( 
    Pool_executor_factory* p, Method* m, Server* s, Server_connection* c 
  ):
    Executor( m, s, c ),
    pool(p)
{
  if( !reactor_interrupter )
    reactor_interrupter = new iqnet::Reactor_interrupter(s->get_reactor());
}


Pool_executor::~Pool_executor()
{
  reactor_interrupter->make_interrupt();
}


void Pool_executor::execute( const Param_list& params_ )
{
  params = params_;
  pool->register_executor( this );
}


void Pool_executor::process_actual_execution()
{
  try {
    Value result(0);
    method->process_execution( interceptors, params, result );
    schedule_response( Response(result) );
  }
  catch( const Fault& f )
  {
    schedule_response( Response( f.code(), f.what() ) );
  }
  catch( const std::exception& e )
  {
    schedule_response( Response( -1, e.what() ) );
  }
  catch( ... )
  {
    schedule_response( Response( -1, "Unknown Error" ) );
  }
}
