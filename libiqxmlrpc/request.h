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
//  $Id: request.h,v 1.3 2004-04-14 08:55:55 adedov Exp $

#ifndef _iqxmlrpc_request_h_
#define _iqxmlrpc_request_h_

#include <string>
#include <vector>
#include <libxml++/libxml++.h>
#include "value.h"


namespace iqxmlrpc 
{
  class Value;
  class Request;
    
  typedef std::vector<Value> Param_list;
  
  //! Build request object from XML-formed string.
  Request* parse_request( const std::string& );
};


//! Incoming RPC request.
class iqxmlrpc::Request {
public:
  typedef Param_list::const_iterator const_iterator;

private:
  std::string name;
  Param_list  params;

public:
  Request( const xmlpp::Document* );
  Request( const xmlpp::Node* );
  Request( const std::string& name, const Param_list& params );
  virtual ~Request();

  xmlpp::Document* to_xml() const;

  const std::string& get_name()   const { return name; }
  const Param_list&  get_params() const { return params; }
  
private:
  void parse( const xmlpp::Node* );
  void parse_name( const xmlpp::Node* );
  void parse_params( const xmlpp::Node* );
};


#endif
