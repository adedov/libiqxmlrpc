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
//  $Id: request.h,v 1.7 2006-09-07 04:45:21 adedov Exp $

#ifndef _iqxmlrpc_request_h_
#define _iqxmlrpc_request_h_

#include <string>
#include <vector>

#include "value.h"

namespace iqxmlrpc {

class Request;
typedef std::vector<Value> Param_list;

//! Build request object from XML-formed string.
LIBIQXMLRPC_API  Request* parse_request( const std::string& );

//! Dump Request to XML.
LIBIQXMLRPC_API std::string dump_request( const Request& );

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

//! Incoming RPC request.
class LIBIQXMLRPC_API Request {
public:
  typedef Param_list::const_iterator const_iterator;

  Request( const std::string& name, const Param_list& params );

  const std::string& get_name()   const { return name; }
  const Param_list&  get_params() const { return params; }

private:
  std::string name;
  Param_list  params;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqxmlrpc

#endif
