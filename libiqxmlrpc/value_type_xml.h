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
//  $Id: value_type_xml.h,v 1.2 2006-09-06 17:10:56 adedov Exp $

#include <string>
#include "value_type_visitor.h"
#include "xml_utils.h"

namespace iqxmlrpc {

//! Value_type visitor that converts values into XML-RPC representation.
class Value_type_to_xml: public Value_type_visitor {
public:
  Value_type_to_xml(XmlBuilder& builder):
    builder_(builder) {}

private:
  virtual void do_visit_value(const Value_type&);
  virtual void do_visit_nil();
  virtual void do_visit_int(int);
  virtual void do_visit_double(double);
  virtual void do_visit_bool(bool);
  virtual void do_visit_string(const std::string&);
  virtual void do_visit_struct(const Struct&);
  virtual void do_visit_array(const Array&);
  virtual void do_visit_base64(const Binary_data&);
  virtual void do_visit_datetime(const Date_time&);

  void add_textnode(const char* name, const std::string& data);

  XmlBuilder& builder_;
};

} // namespace iqxmlrpc
