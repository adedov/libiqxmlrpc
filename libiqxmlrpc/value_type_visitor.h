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
//  $Id: value_type_visitor.h,v 1.2 2006-08-31 17:15:26 adedov Exp $

#ifndef _iqxmlrpc_value_type_visitor_base_
#define _iqxmlrpc_value_type_visitor_base_

#include <iosfwd>
#include "value_type.h"

namespace iqxmlrpc {

class Value_type_visitor {
public:
  virtual ~Value_type_visitor() {}

  void visit_value(const Value_type& v)
  {
    do_visit_value(v);
  }

  void visit_nil()
  {
    do_visit_nil();
  }

  void visit_int(int i)
  {
    do_visit_int(i);
  }

  void visit_double(double d)
  {
    do_visit_double(d);
  }

  void visit_bool(bool b)
  {
    do_visit_bool(b);
  }

  void visit_string(const std::string& s)
  {
    do_visit_string(s);
  }

  void visit_struct(const Struct& s)
  {
    do_visit_struct(s);
  }

  void visit_array(const Array& a)
  {
    do_visit_array(a);
  }

  void visit_base64(const Binary_data& b)
  {
    do_visit_base64(b);
  }

  void visit_datetime(const Date_time& d)
  {
    do_visit_datetime(d);
  }

private:
  virtual void do_visit_value(const Value_type&) = 0;

  virtual void do_visit_nil() = 0;
  virtual void do_visit_int(int) = 0;
  virtual void do_visit_double(double) = 0;
  virtual void do_visit_bool(bool) = 0;
  virtual void do_visit_string(const std::string&) = 0;

  virtual void do_visit_struct(const Struct&) = 0;
  virtual void do_visit_array(const Array&) = 0;
  virtual void do_visit_base64(const Binary_data&) = 0;
  virtual void do_visit_datetime(const Date_time&) = 0;
};


class Print_value_visitor: public Value_type_visitor {
public:
  Print_value_visitor(std::ostream&);

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

  std::ostream& out_;
};

} // namespace iqxmlrpc

#endif
