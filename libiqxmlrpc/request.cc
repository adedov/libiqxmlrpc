//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

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
