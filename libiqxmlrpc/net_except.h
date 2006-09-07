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
//  $Id: net_except.h,v 1.3 2006-09-07 04:45:21 adedov Exp $

#ifndef _libiqnet_net_except_h_
#define _libiqnet_net_except_h_

#include <stdexcept>
#include "api_export.h"
#include "sysinc.h"

namespace iqnet
{

//! Exception class to wrap a network's subsystem errors.
class LIBIQXMLRPC_API network_error: public std::runtime_error {
public:
  network_error( const std::string& msg, bool use_errno = true );
};

} // namespace iqnet

#endif
