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

#include "value_type_xml.h"

#include "utf_conv.h"

#include <boost/lexical_cast.hpp>
#include <libxml++/libxml++.h>

namespace iqxmlrpc {
namespace type_names {
  const std::string nil_type_name     = "nil";
  const std::string int_type_name     = "i4";
  const std::string bool_type_name    = "boolean";
  const std::string double_type_name  = "double";
  const std::string string_type_name  = "string";
  const std::string array_type_name   = "array";
  const std::string struct_type_name  = "struct";
  const std::string base64_type_name  = "base64";
  const std::string date_type_name    = "dateTime.iso8601";
} // namespace type_names

void Value_type_to_xml::do_visit_value(const Value_type& v)
{
  xmlpp::Element* el = parent_->add_child( "value" );
  Value_type_to_xml vis(el);
  v.apply_visitor(vis);
}

void Value_type_to_xml::do_visit_nil()
{
  parent_->add_child(type_names::nil_type_name);
}

void Value_type_to_xml::do_visit_int(int val)
{
  add_child_with_content(
    type_names::int_type_name, boost::lexical_cast<std::string>(val));
}

void Value_type_to_xml::do_visit_double(double val)
{
  add_child_with_content(
    type_names::double_type_name, boost::lexical_cast<std::string>(val));
}

void Value_type_to_xml::do_visit_bool(bool val)
{
  add_child_with_content(
    type_names::bool_type_name, val ? "1" : "0");
}

void Value_type_to_xml::do_visit_string(const std::string& val)
{
  add_child_with_content(
    type_names::string_type_name, config::cs_conv->to_utf(val));
}

void Value_type_to_xml::do_visit_struct(const Struct& s)
{
  xmlpp::Element* str_el = parent_->add_child(type_names::struct_type_name);

  typedef Struct::const_iterator CI;
  for(CI i = s.begin(); i != s.end(); ++i )
  {
    xmlpp::Element* mbr_el = str_el->add_child( "member" );
    xmlpp::Element* name_el = mbr_el->add_child( "name" );
    name_el->add_child_text( i->first );

    Value_type_to_xml vis(mbr_el);
    i->second->apply_visitor(vis);
  }
}

void Value_type_to_xml::do_visit_array(const Array& a)
{
  xmlpp::Element* arr_el = parent_->add_child( type_names::array_type_name );
  xmlpp::Element* el = arr_el->add_child( "data" );

  typedef Array::const_iterator CI;
  Value_type_to_xml vis(el);

  for(CI i = a.begin(); i != a.end(); ++i ) {
    i->apply_visitor(vis);
  }
}

void Value_type_to_xml::do_visit_base64(const Binary_data& bin)
{
  add_child_with_content(
    type_names::base64_type_name, bin.get_base64());
}

void Value_type_to_xml::do_visit_datetime(const Date_time& d)
{
  add_child_with_content(
    type_names::date_type_name, d.to_string());
}

//
// utils
//

inline
void Value_type_to_xml::add_child_with_content(
  const std::string& name, const std::string& cont)
{
  parent_->add_child(name)->add_child_text(cont);
}

} // namespace iqxmlrpc
