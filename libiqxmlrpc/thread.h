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
//  $Id: thread.h,v 1.2 2004-04-27 05:25:09 adedov Exp $

#ifndef _libiqnet_thread_h_
#define _libiqnet_thread_h_

#include <stdexcept>
#include "sysinc.h"
#include "lock.h"

namespace iqnet
{
  class Thread;
};


//! Thread wrapper.
class iqnet::Thread {
public:
  //! Unable to create a thread exception.
  class Create_failed: public std::runtime_error {
  public:
    Create_failed():
      runtime_error( "Could not create a thread." ) {}
  };
  
private:
  pthread_t thr;
  Mutex_lock init_lock;
  
public:
  Thread();
  //! Cancels thread.
  virtual ~Thread();

  void join();
  void cancel();

private:
  static void* run( void* obj );

protected:
  virtual void do_run() = 0;
};

#endif
