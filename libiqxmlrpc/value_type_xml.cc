//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include <boost/lexical_cast.hpp>

#include "value.h"
#include "value_type_xml.h"
#include "xml_builder.h"

namespace iqxmlrpc {

typedef XmlBuilder::Node XmlNode;

inline void
Value_type_to_xml::add_textnode(const char* name, const std::string& cont)
{
  XmlNode n(builder_, name);
  n.set_textdata(cont);
}

void Value_type_to_xml::do_visit_value(const Value_type& v)
{
  XmlNode value(builder_, "value");
  v.apply_visitor(*this);
}

void Value_type_to_xml::do_visit_nil()
{
  XmlNode(builder_, "nil");
}

void Value_type_to_xml::do_visit_int(int val)
{
  add_textnode("i4", boost::lexical_cast<std::string>(val));
}

void Value_type_to_xml::do_visit_double(double val)
{
  add_textnode("double", boost::lexical_cast<std::string>(val));
}

void Value_type_to_xml::do_visit_bool(bool val)
{
  add_textnode("boolean", val ? "1" : "0");
}

void Value_type_to_xml::do_visit_string(const std::string& val)
{
  add_textnode("string", val);
}

void Value_type_to_xml::do_visit_struct(const Struct& s)
{
  XmlNode st(builder_, "struct");

  typedef Struct::const_iterator CI;
  for(CI i = s.begin(); i != s.end(); ++i )
  {
    XmlNode member(builder_, "member");
    add_textnode("name", i->first);

    Value_type_to_xml vis(builder_);
    i->second->apply_visitor(vis);
  }
}

void Value_type_to_xml::do_visit_array(const Array& a)
{
  XmlNode arr(builder_, "array");
  XmlNode data(builder_, "data");

  typedef Array::const_iterator CI;
  Value_type_to_xml vis(builder_);

  for(CI i = a.begin(); i != a.end(); ++i ) {
    i->apply_visitor(vis);
  }
}

void Value_type_to_xml::do_visit_base64(const Binary_data& bin)
{
  add_textnode("base64", bin.get_base64());
}

void Value_type_to_xml::do_visit_datetime(const Date_time& d)
{
  add_textnode("dateTime.iso8601", d.to_string());
}

} // namespace iqxmlrpc
