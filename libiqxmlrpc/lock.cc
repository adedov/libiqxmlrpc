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
//  $Id: lock.cc,v 1.1 2004-04-27 04:10:52 adedov Exp $

#include "lock.h"

using namespace iqnet;


Mutex_lock::Mutex_lock()
{
  pthread_mutex_init( &mutex, 0 );
}


Mutex_lock::~Mutex_lock()
{
  release();
  pthread_mutex_destroy( &mutex );
}


void Mutex_lock::acquire()
{
  pthread_mutex_lock( &mutex );
}


void Mutex_lock::release()
{
  pthread_mutex_unlock( &mutex );
}


// ----------------------------------------------------------------------------
Cond::Cond()
{
  pthread_mutex_init( &mutex, 0 );
  pthread_cond_init( &cond, 0 );
}


Cond::~Cond()
{
  pthread_cond_destroy( &cond );
  pthread_mutex_destroy( &mutex );
}


void Cond::acquire_lock()
{
  pthread_mutex_lock( &mutex );
}


void Cond::release_lock()
{
  pthread_mutex_unlock( &mutex );
}


void Cond::wait()
{
  acquire_lock();
  pthread_cond_wait( &cond, &mutex );
}


void Cond::signal()
{
  pthread_cond_signal( &cond );
}
