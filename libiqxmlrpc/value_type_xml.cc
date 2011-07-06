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
//  $Id: value_type_xml.cc,v 1.1 2006-08-30 18:01:36 adedov Exp $

#include <boost/lexical_cast.hpp>

#include "value.h"
#include "value_type_xml.h"

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
