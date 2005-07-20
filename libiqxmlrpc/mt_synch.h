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
//  $Id: mt_synch.h,v 1.2 2005-07-20 17:09:03 bada Exp $

#ifndef _libiqnet_mt_synch_h_
#define _libiqnet_mt_synch_h_

#include "lock.h"

namespace iqnet
{
  class Mutex_lock;
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

#endif
