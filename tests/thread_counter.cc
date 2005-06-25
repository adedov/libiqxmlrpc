#include "thread_counter.h"

Thread_counter::Thread_counter()
{
}

Thread_counter::~Thread_counter()
{
}

void Thread_counter::operator ++()
{
  iqnet::Auto_lock lock(&cnt_mutex_);
  ++cnt_;
}

void Thread_counter::operator --()
{
  iqnet::Auto_lock lock(&cnt_mutex_);
  if (--cnt_ <= 0)
    zero_cond_.signal();
}

void Thread_counter::wait_for_zero()
{
  zero_cond_.wait();
}
