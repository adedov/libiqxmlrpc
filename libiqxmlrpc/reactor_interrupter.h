//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2006 Anton Dedov
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
//  $Id: reactor_interrupter.h,v 1.4 2006-09-07 09:29:46 adedov Exp $

#ifndef _iqxmlrpc_reactor_interrupter_h_
#define _iqxmlrpc_reactor_interrupter_h_

#include <boost/utility.hpp>
#include "api_export.h"
#include "reactor.h"

namespace iqnet {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4275)
#endif

class LIBIQXMLRPC_API Reactor_interrupter: boost::noncopyable {
public:
  Reactor_interrupter(Reactor_base*);
  ~Reactor_interrupter();

  void make_interrupt();

private:
  class Impl;
  Impl* impl_;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqnet

#endif
