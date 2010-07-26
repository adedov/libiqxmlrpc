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
//  $Id: value_type_visitor.cc,v 1.1 2006-08-31 17:15:26 adedov Exp $

#include "value_type_visitor.h"

#include "value.h"

#include <iostream>

namespace iqxmlrpc {

Print_value_visitor::Print_value_visitor(std::ostream& out):
  out_(out)
{
}

void Print_value_visitor::do_visit_value(const Value_type& v)
{
  v.apply_visitor(*this);
}

void Print_value_visitor::do_visit_nil()
{
  out_ << "NIL";
}

void Print_value_visitor::do_visit_int(int val)
{
  out_ << val;
}

void Print_value_visitor::do_visit_double(double val)
{
  out_ << val;
}

void Print_value_visitor::do_visit_bool(bool val)
{
  out_ << val;
}

void Print_value_visitor::do_visit_string(const std::string& val)
{
  out_ << "'" << val << "'";
}

void Print_value_visitor::do_visit_struct(const Struct& s)
{
  out_ << "{";

  typedef Struct::const_iterator CI;
  for(CI i = s.begin(); i != s.end(); ++i )
  {
    out_ << " '" << i->first << "' => ";
    i->second->apply_visitor(*this);
    out_ << ",";
  }

  out_ << " }";
}

void Print_value_visitor::do_visit_array(const Array& a)
{
  out_ << "[";

  typedef Array::const_iterator CI;
  for(CI i = a.begin(); i != a.end(); ++i )
  {
    out_ << " ";
    i->apply_visitor(*this);
    out_ << ",";
  }

  out_ << " ]";
}

void Print_value_visitor::do_visit_base64(const Binary_data& bin)
{
  out_ << "RAWDATA";
}

void Print_value_visitor::do_visit_datetime(const Date_time& d)
{
  do_visit_string(d.to_string());
}

} // namespace iqxmlrpc
