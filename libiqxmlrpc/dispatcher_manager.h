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
//  $Id: dispatcher_manager.h,v 1.2 2006-09-07 04:45:21 adedov Exp $

#ifndef _iqxmlrpc_dispatcher_manager_
#define _iqxmlrpc_dispatcher_manager_

#include <boost/noncopyable.hpp>
#include "api_export.h"
#include "method.h"

namespace iqxmlrpc {

//! Class that encapsulate control of multiple method dispatchers.
/*! It also provides default method dispatcher that support usual
 *  register_method operation and optionally system one, which holds
 *  server's built-in methods
 */
class LIBIQXMLRPC_API Method_dispatcher_manager: boost::noncopyable {
  class Impl;
  Impl* impl_;

public:
  Method_dispatcher_manager();
  ~Method_dispatcher_manager();

  //! Registers method factory in default method dispatcher.
  void register_method(const std::string& name, Method_factory_base*);

  //! Add dispatcher at the end of dispatchers list. Grabs ownership.
  void push_back(Method_dispatcher_base*);

  //! Create method.
  /*! It calls each registered method dispatcher in the same order as
      they registered. First non-zero result will be returned.
   */
  Method* create_method(const Method::Data&);

  //! Return list of methods provided by all registered dispatchers.
  void get_methods_list(Array&) const;

  //! Turns on introspection.
  void enable_introspection();
};

} // namespace iqxmlrpc

#endif
