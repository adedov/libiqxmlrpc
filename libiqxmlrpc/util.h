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
//  $Id: util.h,v 1.1 2005-03-29 16:30:59 bada Exp $

#ifndef _iqxmlrpc_util_h_
#define _iqxmlrpc_util_h_

#include <functional>

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

} // namespace util
} // namespace iqxmlrpc

#endif
