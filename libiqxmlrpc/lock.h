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
//  $Id: lock.h,v 1.2 2004-04-27 04:12:32 adedov Exp $

#ifndef _libiqnet_lock_h_
#define _libiqnet_lock_h_

#include "sysinc.h"


namespace iqnet 
{
  class Lock;
  class Null_lock;
  class Mutex_lock;
  class Auto_lock;
    
  class Cond;
};


//! Abstract synchronization class.
class iqnet::Lock {
public:
  virtual ~Lock() {}

  virtual void acquire() = 0;
  virtual void release() = 0;
};


//! Class which provides null synchronization.
class iqnet::Null_lock: public iqnet::Lock {
public:
  void acquire() {}
  void release() {}
};


//! Thread synchronization via mutex
class iqnet::Mutex_lock: public iqnet::Lock {
  pthread_mutex_t mutex;

public:
  Mutex_lock();
  ~Mutex_lock();

  void acquire();
  void release();
};


//! Auto lock/unlock.
class iqnet::Auto_lock {
  iqnet::Lock* lock;
  
public:
  //! Aquire lock during the construction.
  Auto_lock( iqnet::Lock* lock_ ): lock(lock_) 
  {
    lock->acquire();
  }
  
  //! Release lock when object is destroyed.
  ~Auto_lock()
  {
    lock->release();
  }
  
  void acquire()
  {
    lock->acquire();
  }
  
  void release()
  {
    lock->release();
  }
};


class iqnet::Cond {
  pthread_cond_t cond;
  pthread_mutex_t mutex;

public:
  Cond();
  virtual ~Cond();

  void acquire_lock();
  void release_lock();  

  void wait();
  void signal();
};

#endif
