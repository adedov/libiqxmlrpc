#include <iostream>
#include "mt.h"

using namespace iqnet;


// --------- Mutex -----------
Mutex::Mutex()
{
  pthread_mutex_init( &mutex, 0 );
}


Mutex::~Mutex()
{
  pthread_mutex_destroy( &mutex );
}


void Mutex::lock()
{
  pthread_mutex_lock( &mutex );
}


void Mutex::unlock()
{
  pthread_mutex_unlock( &mutex );
}


bool Mutex::trylock()
{
  if( pthread_mutex_trylock( &mutex ) == EBUSY )
    return false;
    
  return true;
}


// ---------- Cond -----------------
Cond::Cond( Mutex* mtx ):
  mutex(mtx)
{
  pthread_cond_init( &cond, 0 );
}


Cond::~Cond()
{
  pthread_cond_destroy( &cond );
}


void Cond::signal()
{
  pthread_cond_signal( &cond );
}


void Cond::wait()
{
  pthread_cond_wait( &cond, &mutex->mutex );
  mutex->unlock();
}


// ---------- Thread ---------------
void* Thread::run_thread( void* arg )
{
  Thread *obj = reinterpret_cast<Thread*>(arg);
  obj->run();

  delete obj;
  return 0;
}


Thread::Thread():
  thrid(0), exit_flag_(false)
{
}


Thread::~Thread()
{
}


void Thread::activate()
{
  pthread_create( &thrid, 0, Thread::run_thread, this );
}


void Thread::set_exit_flag()
{
  exit_flag_mutex.lock();
  exit_flag_ = true;
  exit_flag_mutex.unlock();
}


bool Thread::exit_flag()
{
  bool retval = false;
  
  exit_flag_mutex.lock();
  retval = exit_flag_;
  exit_flag_mutex.unlock();

  return retval;
}


