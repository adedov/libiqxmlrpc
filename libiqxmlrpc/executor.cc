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
//  $Id: executor.cc,v 1.7 2005-03-29 16:30:58 bada Exp $

#include "executor.h"
#include "except.h"
#include "response.h"
#include "server.h"
#include "util.h"

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
  try {
    Value result(0);
    method->execute( params, result );
    schedule_response( Response(result) );
  }
  catch( const Fault& f )
  {
    schedule_response( Response( f.code(), f.what() ) );
  }
}


// ----------------------------------------------------------------------------
#ifndef DOXYGEN_SHOULD_SKIP_THIS
class Pool_executor_factory::Pool_thread: public iqnet::Thread {
  unsigned name;
  Pool_executor_factory* pool;

public:
  Pool_thread( unsigned name_, Pool_executor_factory* pool_ ):
    name(name_),
    pool(pool_) 
  {
  }

protected:
  void do_run();
};
#endif


void Pool_executor_factory::Pool_thread::do_run()
{
  Pool_executor_factory::Pool_thread* obj = 
    static_cast<Pool_executor_factory::Pool_thread*>(this);
  
  Pool_executor_factory* pool = obj->pool;

  for(;;)
  {
    pool->req_queue_cond.wait();

    if( pool->req_queue.empty() )
      continue;

    Pool_executor* executor = pool->req_queue.front();
    pool->req_queue.pop_front();
    pool->req_queue_cond.release_lock();

    executor->process_actual_execution();
  }
}


// ----------------------------------------------------------------------------
Pool_executor_factory::Pool_executor_factory( unsigned pool_size )
{
  pool.reserve( pool_size );
  for( unsigned i = 0; i < pool_size; ++i )
    pool.push_back( new Pool_thread(i, this) );
}


Pool_executor_factory::~Pool_executor_factory()
{
  util::delete_ptrs(pool.begin(), pool.end());
  req_queue_cond.acquire_lock();
  util::delete_ptrs(req_queue.begin(), req_queue.end());
}


void Pool_executor_factory::register_executor( Pool_executor* executor )
{
  req_queue_cond.acquire_lock();
  req_queue.push_back( executor );
  req_queue_cond.signal();
  req_queue_cond.release_lock();
}


// ----------------------------------------------------------------------------
iqnet::Alarm_socket* Pool_executor::alarm_sock = 0;


Pool_executor::Pool_executor( 
    Pool_executor_factory* p, Method* m, Server* s, Server_connection* c 
  ):
    Executor( m, s, c ),
    pool(p)
{
  if( !alarm_sock )
    alarm_sock = new iqnet::Alarm_socket( s->get_reactor(), new Lock );
}


Pool_executor::~Pool_executor()
{
  alarm_sock->send_alarm();
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
    method->execute( params, result );
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
