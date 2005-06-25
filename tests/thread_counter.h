#ifndef _libiqxmlrpc_test_suite_thread_counter_h_
#define _libiqxmlrpc_test_suite_thread_counter_h_

#include <boost/utility.hpp>
#include "libiqxmlrpc/mt_synch.h"

class Thread_counter: public boost::noncopyable {
  int cnt_;
  iqnet::Mutex_lock cnt_mutex_;
  iqnet::Cond       zero_cond_;
  
public:
  Thread_counter();
  ~Thread_counter();

  void operator ++();
  void operator --();

  void wait_for_zero();
};

class Thread_counter_dec {
  Thread_counter& counter_;
  
public:
  Thread_counter_dec(Thread_counter& c):
    counter_(c)
  {
  }

  ~Thread_counter_dec()
  {
    --counter_;
  }
};

#endif
