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
//  $Id: builtins.h,v 1.4 2006-09-07 04:45:21 adedov Exp $

#ifndef _iqxmlrpc_buildins_h_
#define _iqxmlrpc_buildins_h_

#include "method.h"

namespace iqxmlrpc {

class Method_dispatcher_manager;

namespace builtins {

//! Implementation of system.listMethods
//! See http://xmlrpc.usefulinc.com/doc/reserved.html
class LIBIQXMLRPC_API List_methods: public Method {
  Method_dispatcher_manager* disp_manager_;

public:
  List_methods(Method_dispatcher_manager*);

private:
  void execute( const Param_list& params, Value& response );
};

} // namespace builtins
} // namespace iqxmlrpc

#endif
