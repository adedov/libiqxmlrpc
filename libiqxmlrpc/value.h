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
//  $Id: value.h,v 1.23 2006-09-07 04:45:21 adedov Exp $

#ifndef _iqxmlrpc_value_h_
#define _iqxmlrpc_value_h_

#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <iosfwd>
#include "api_export.h"
#include "value_type.h"
#include "except.h"

namespace xmlpp {
  class Node;
}

namespace iqxmlrpc
{

//! Proxy class to access XML-RPC values by library users.
/*! \exception Bad_cast */
class LIBIQXMLRPC_API Value {
public:
  //! Bad_cast is being thrown on illegal
  //! type conversion or Value::get_X() call.
  class Bad_cast: public Exception {
  public:
    Bad_cast();
  };

private:
  Value_type* value;

public:
  Value( Value_type* );
  Value( const Value& );
  Value( Nil );
  Value( int );
  Value( bool );
  Value( double );
  Value( std::string );
  Value( const char* );
  Value( const Binary_data& );
  Value( const Date_time& );
  Value( const struct tm* );
  Value( const Array& );
  Value( const Struct& );

  virtual ~Value();

  const Value& operator =( const Value& );

  //! \name Type identification
  //! \{
  bool is_nil()    const;
  bool is_int()    const;
  bool is_bool()   const;
  bool is_double() const;
  bool is_string() const;
  bool is_binary() const;
  bool is_datetime() const;
  bool is_array()  const;
  bool is_struct() const;

  const std::string& type_name() const;
  //! \}

  //! \name Access scalar value
  //! \{
  int         get_int()    const;
  bool        get_bool()   const;
  double      get_double() const;
  std::string get_string() const;
  Binary_data get_binary() const;
  Date_time   get_datetime() const;

  operator int()         const;
  operator bool()        const;
  operator double()      const;
  operator std::string() const;
  operator Binary_data() const;
  operator struct tm()   const;
  //! \}

  //! \name Array functions
  //! \{
  //! Access inner Array value
  Array& the_array();
  const Array& the_array() const;

  unsigned size() const;
  const Value& operator []( int ) const;
  Value&       operator []( int );

  void push_back( const Value& v );

  Array::const_iterator arr_begin() const;
  Array::const_iterator arr_end() const;
  //! \}

  //! \name Struct functions
  //! \{
  //! Access inner Struct value.
  Struct& the_struct();
  const Struct& the_struct() const;

  bool has_field( const std::string& f ) const;

  const Value& operator []( const char* ) const;
  Value&       operator []( const char* );
  const Value& operator []( const std::string& ) const;
  Value&       operator []( const std::string& );

  void insert( const std::string& n, const Value& v );
  //! \}

  void apply_visitor(Value_type_visitor&) const;

private:
  template <class T> T* cast() const;
  template <class T> bool can_cast() const;
};

void LIBIQXMLRPC_API value_to_xml(const Value&, xmlpp::Node*);
void LIBIQXMLRPC_API print_value(const Value&, std::ostream&);

} // namespace iqxmlrpc

#endif
