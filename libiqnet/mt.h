#ifndef _libiqnet_mt_h_
#define _libiqnet_mt_h_

#include <pthread.h>


namespace iqnet 
{
  class Mutex;
  class Cond;
  class Thread;
};


//! Mutual exclusion blocking class.
/*! Wrapper for pthread_mutex_t. */
class iqnet::Mutex {
  pthread_mutex_t mutex;

  friend class iqnet::Cond;
    
public:
  Mutex();
  virtual ~Mutex();
  
  void lock();
  void unlock();
  bool trylock();
};


/*!
\class iqnet::Cond
\brief Condition variable.

Wrapper for pthread_cont_t.
*/
class iqnet::Cond {
  iqnet::Mutex* mutex;
  pthread_cond_t cond;
  
public:
  Cond( iqnet::Mutex* );
  virtual ~Cond();
  
  void signal();
  void wait();
};


//! Abstract thread of execution.
/*! Wrapper for pthread_t. */
class iqnet::Thread {
  pthread_t thrid;
  
  bool exit_flag_;
  iqnet::Mutex exit_flag_mutex;
  
public:
  Thread();
  virtual ~Thread();

  //! Start new thread of execution.
  /*!
      activate() starts running function run()
      in new thread of execution.
      
      \warning Do not call this from constructor!
  */
  void activate();

  void set_exit_flag();
  bool exit_flag();
    
protected:
//  void join();

  //! Reload it in inherited class to provide functionality.
  /*!\a Example:
   \code
    void SomeThreadsChild::run()
    {
      while( !exit_flag() )
        ...
    }
   \endcode
  */
  virtual void run() = 0;
  
private:
  static void* run_thread( void* );
};

#endif
