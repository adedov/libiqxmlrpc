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
//  $Id: response.h,v 1.2 2004-03-29 06:23:18 adedov Exp $

#ifndef _iqxmlrpc_response_h_
#define _iqxmlrpc_response_h_

#include <string>
#include <libxml++/libxml++.h>


namespace iqxmlrpc 
{
  class Value;
  class Response;
};


//! XML-RPC response.
class iqxmlrpc::Response {
private:
  Value* value_;
  int fault_code_;
  std::string fault_string_;
  
public:
  Response( const xmlpp::Document* );
  Response( const xmlpp::Node* );
  Response( const Value& );
  Response( int fault_code, const std::string& fault_string );
  virtual ~Response();

  xmlpp::Document* to_xml() const;

  //! Returns response value or throws Fault in case of fault.
  const Value& value() const;

  bool is_fault()   const { return !value_; }
  int  fault_code() const { return fault_code_; }
  const std::string& fault_string() const { return fault_string_; }
  
private:
  void parse( const xmlpp::Node* );
  void parse_param( const xmlpp::Node* );
  void parse_fault( const xmlpp::Node* );

  void ok_to_xml( xmlpp::Node* ) const;
  void fault_to_xml( xmlpp::Node* ) const;
};

#endif
