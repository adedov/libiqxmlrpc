//  Libiqxmlrpc - an object-oriented XML-RPC solution.
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
//  $Id: util.h,v 1.4 2005-09-20 16:03:00 bada Exp $

#ifndef _iqxmlrpc_util_h_
#define _iqxmlrpc_util_h_

#include <functional>
#include <memory>
#include <boost/utility.hpp>
#include "libiqxmlrpc/lock.h"

namespace iqxmlrpc
{

//! Utility stuff
namespace util
{

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
