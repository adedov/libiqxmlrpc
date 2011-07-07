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
//  $Id: request.cc,v 1.8 2006-09-07 09:35:42 adedov Exp $

#include <boost/foreach.hpp>

#include "request.h"
#include "request_parser.h"

#include "except.h"
#include "value.h"
#include "value_type_xml.h"
#include "xml_builder.h"

namespace iqxmlrpc {

Request*
parse_request( const std::string& request_string )
{
  Parser parser(request_string);
  RequestBuilder builder(parser);
  builder.build();
  return builder.get();
}

std::string
dump_request(const Request& request)
{
  XmlBuilder writer;
  XmlBuilder::Node root(writer, "methodCall");

  {
    XmlBuilder::Node name(writer, "methodName");
    name.set_textdata(request.get_name());
  }

  {
    XmlBuilder::Node params(writer, "params");
    BOOST_FOREACH(const Value& v, request.get_params()) {
      XmlBuilder::Node param(writer, "param");
      value_to_xml(writer, v);
    }
  }

  writer.stop();
  return writer.content();
}

//
// Request
//

Request::Request( const std::string& name_, const Param_list& params_ ):
  name(name_),
  params(params_)
{
}

} // namespace iqxmlrpc
