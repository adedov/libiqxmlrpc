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
//  $Id: reactor_select_impl.h,v 1.2 2005-09-24 16:24:57 bada Exp $

#ifndef _iqxmlrpc_reactor_select_impl_h_
#define _iqxmlrpc_reactor_select_impl_h_

#include "config.h"

#ifndef HAVE_POLL
#include <boost/utility.hpp>
#include "reactor.h"
#include "sysinc.h"

namespace iqnet
{

//! Reactor implementation helper based on select() system call.
class Reactor_select_impl: boost::noncopyable {
  Socket::Handler max_fd;
  fd_set read_set, write_set, err_set;
  Reactor_base::HandlerStateList hs;

public:
  Reactor_select_impl();
  virtual ~Reactor_select_impl();
  
  void reset(const Reactor_base::HandlerStateList&);
  bool poll(Reactor_base::HandlerStateList& out, Reactor_base::Timeout);
};

} // namespace iqnet

#endif // not defined HAVE_POLL
#endif
