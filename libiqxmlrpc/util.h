//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_util_h_
#define _iqxmlrpc_util_h_

#include "lock.h"

#include <boost/utility.hpp>

#include <functional>
#include <memory>

namespace iqxmlrpc {
namespace util {

template <class M>
class Select2nd:
  public std::unary_function<typename M::value_type, typename M::mapped_type>
{
public:
  typename M::mapped_type operator ()(typename M::value_type& i)
  {
    return i.second;
  }
};

template <class Iter>
void delete_ptrs(Iter first, Iter last)
{
  for(; first != last; ++first)
    delete *first;
}

template <class Iter, class AccessOp>
void delete_ptrs(Iter first, Iter last, AccessOp op)
{
  for(; first != last; ++first)
    delete op(*first);
}

template <class Ptr>
class ExplicitPtr {
  Ptr p_;

public:
  explicit ExplicitPtr(Ptr p): p_(p) {}

  ExplicitPtr(ExplicitPtr& ep):
    p_(ep.release())
  {
  }

  ~ExplicitPtr()
  {
    delete release();
  }

  Ptr release()
  {
    Ptr p(p_);
    p_ = 0;
    return p;
  }
};

//! Provides serialized access to some bool value
template <class Lock>
class LockedBool: boost::noncopyable {
  bool val;
  Lock lock;

public:
  LockedBool(bool default_):
    val(default_) {}

  ~LockedBool() {}

  operator bool()
  {
    typename Lock::scoped_lock lk(lock);
    return val;
  }

  void operator =(bool b)
  {
    typename Lock::scoped_lock lk(lock);
    val = b;
  }
};

} // namespace util
} // namespace iqxmlrpc

#endif
