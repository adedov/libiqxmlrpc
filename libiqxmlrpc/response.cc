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
//  $Id: response.cc,v 1.13 2006-09-07 09:35:42 adedov Exp $

#include <boost/foreach.hpp>

#include "response.h"
#include "response_parser.h"

#include "except.h"
#include "value.h"
#include "value_type_xml.h"

namespace iqxmlrpc {

Response
parse_response( const std::string& response_string )
{
  Parser parser(response_string);
  ResponseBuilder builder(parser);
  builder.build();
  return builder.get();
}

std::string
dump_response( const Response& response )
{
  XmlBuilder writer;
  XmlBuilder::Node root(writer, "methodResponse");

  if (!response.is_fault()) {
    XmlBuilder::Node params(writer, "params");
    XmlBuilder::Node param(writer, "param");
    value_to_xml(writer, response.value());
  } else {
    Struct fault;
    fault.insert( "faultCode", response.fault_code() );
    fault.insert( "faultString", response.fault_string() );
    Value v( fault );
    value_to_xml(writer, fault);
  }

  writer.stop();
  return writer.content();
}

//
// Response
//

Response::Response( Value* v ):
  value_(v)
{
}

Response::Response( int fcode, const std::string& fstring ):
  fault_code_(fcode),
  fault_string_(fstring)
{
}

const Value& Response::value() const
{
  if( is_fault() )
    throw iqxmlrpc::Exception( fault_string_, fault_code_ );

  return *value_;
}

} // namespace iqxmlrpc
