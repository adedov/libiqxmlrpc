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
    Bad_cast():
      Exception( "iqxmlrpc::Value: incorrect type was requested." ) {}
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
  bool is_nil()    const { return can_cast<Nil>(); }
  bool is_int()    const { return can_cast<Int>(); }
  bool is_bool()   const { return can_cast<Bool>(); }
  bool is_double() const { return can_cast<Double>(); }
  bool is_string() const { return can_cast<String>(); }
  bool is_binary() const { return can_cast<Binary_data>(); }
  bool is_datetime() const { return can_cast<Date_time>(); }
  bool is_array()  const { return can_cast<Array>(); }
  bool is_struct() const { return can_cast<Struct>(); }

  const std::string& type_name() const { return value->type_name(); }
  //! \}

  //! \name Access scalar value
  //! \{
  int         get_int()    const { return cast<Int>()->value(); }
  bool        get_bool()   const { return cast<Bool>()->value(); }
  double      get_double() const { return cast<Double>()->value(); }
  std::string get_string() const { return cast<String>()->value(); }
  Binary_data get_binary() const { return Binary_data(*cast<Binary_data>()); }
  Date_time   get_datetime() const { return Date_time(*cast<Date_time>()); }

  operator int()         const { return get_int(); }
  operator bool()        const { return get_bool(); }
  operator double()      const { return get_double(); }
  operator std::string() const { return get_string(); }
  operator Binary_data() const { return get_binary(); }
  operator struct tm()   const { return get_datetime().get_tm(); }
  //! \}

  //! \name Array functions
  //! \{
  //! Access inner Array value
  Array& the_array() { return *cast<Array>(); }
  const Array& the_array() const { return *cast<Array>(); }

  unsigned size() const { return cast<Array>()->size(); }
  const Value& operator []( int ) const;
  Value&       operator []( int );

  void push_back( const Value& v ) { cast<Array>()->push_back(v); }

  Array::const_iterator arr_begin() const { return cast<Array>()->begin(); }
  Array::const_iterator arr_end()   const { return cast<Array>()->end(); }
  //! \}

  //! \name Struct functions
  //! \{
  //! Access inner Struct value.
  Struct& the_struct() { return *cast<Struct>(); }
  const Struct& the_struct() const { return *cast<Struct>(); }

  bool has_field( const std::string& f ) const
  {
    return cast<Struct>()->has_field(f);
  }

  const Value& operator []( const char* ) const;
  Value&       operator []( const char* );
  const Value& operator []( const std::string& ) const;
  Value&       operator []( const std::string& );

  void insert( const std::string& n, const Value& v )
  {
    cast<Struct>()->insert(n,v);
  }
  //! \}

  void apply_visitor(Value_type_visitor&) const;

private:
  template <class T> T* cast() const
  {
    T* t = dynamic_cast<T*>( value );
    if( !t )
      throw Bad_cast();
    return t;
  }

  template <class T> bool can_cast() const
  {
    return dynamic_cast<T*>( value );
  }
};

void LIBIQXMLRPC_API value_to_xml(const Value&, xmlpp::Node*);
void LIBIQXMLRPC_API print_value(const Value&, std::ostream&);

} // namespace iqxmlrpc

#endif
