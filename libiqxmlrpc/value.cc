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
//  $Id: value.cc,v 1.12 2006-09-07 09:35:42 adedov Exp $

#include <stdexcept>
#include "value.h"
#include "value_type_visitor.h"
#include "value_type_xml.h"

namespace iqxmlrpc {

Value::Bad_cast::Bad_cast():
  Exception( "iqxmlrpc::Value: incorrect type was requested." ) {}


Value::Value( Value_type* v ):
  value(v)
{
}

Value::Value( const Value& v ):
  value( v.value->clone() )
{
}

Value::Value( Nil n ):
  value( n.clone() )
{
}

Value::Value( int i ):
  value( new Int(i) )
{
}

Value::Value( bool b ):
  value( new Bool(b) )
{
}

Value::Value( double d ):
  value( new Double(d) )
{
}

Value::Value( std::string s ):
  value( new String(s) )
{
}

Value::Value( const char* s ):
  value( new String(s) )
{
}

Value::Value( const Array& arr ):
  value( arr.clone() )
{
}

Value::Value( const Struct& st ):
  value( st.clone() )
{
}

Value::Value( const Binary_data& bin ):
  value( bin.clone() )
{
}

Value::Value( const Date_time& dt ):
  value( dt.clone() )
{
}

Value::Value( const struct tm* dt ):
  value( new Date_time(dt) )
{
}

Value::~Value()
{
  delete value;
}

template <class T>
T* Value::cast() const
{
  T* t = dynamic_cast<T*>( value );
  if( !t )
    throw Bad_cast();
  return t;
}

template <class T>
bool Value::can_cast() const
{
  return dynamic_cast<T*>( value );
}

const Value& Value::operator =( const Value& v )
{
  Value_type* tmp = v.value->clone();
  delete value;
  value = tmp;
  return *this;
}

bool Value::is_nil() const
{
  return can_cast<Nil>();
}

bool Value::is_int() const
{
  return can_cast<Int>();
}

bool Value::is_bool() const
{
  return can_cast<Bool>();
}

bool Value::is_double() const
{
  return can_cast<Double>();
}

bool Value::is_string() const
{
  return can_cast<String>();
}

bool Value::is_binary() const
{
  return can_cast<Binary_data>();
}

bool Value::is_datetime() const
{
  return can_cast<Date_time>();
}

bool Value::is_array() const
{
  return can_cast<Array>();
}

bool Value::is_struct() const
{
  return can_cast<Struct>();
}
const std::string& Value::type_name() const
{
  return value->type_name();
}

int Value::get_int() const
{
  return cast<Int>()->value();
}

bool Value::get_bool() const
{
  return cast<Bool>()->value();
}

double Value::get_double() const
{
  return cast<Double>()->value();
}

std::string Value::get_string() const
{
  return cast<String>()->value();
}

Binary_data Value::get_binary() const
{
  return Binary_data(*cast<Binary_data>());
}

Date_time Value::get_datetime() const
{
  return Date_time(*cast<Date_time>());
}

Value::operator int() const
{
  return get_int();
}

Value::operator bool() const
{
  return get_bool();
}

Value::operator double() const
{
  return get_double();
}

Value::operator std::string() const
{
  return get_string();
}

Value::operator Binary_data() const
{
  return get_binary();
}

Value::operator struct tm() const
{
  return get_datetime().get_tm();
}

Array& Value::the_array()
{
  return *cast<Array>();
}

const Array& Value::the_array() const
{
  return *cast<Array>();
}

unsigned Value::size() const
{
  return cast<Array>()->size();
}

void Value::push_back( const Value& v )
{
  cast<Array>()->push_back(v);
}

const Value& Value::operator []( int i ) const
{
  const Array *a = cast<Array>();
  return (*a)[i];
}

Value& Value::operator []( int i )
{
  return (*cast<Array>())[i];
}

Array::const_iterator Value::arr_begin() const
{
  return cast<Array>()->begin();
}

Array::const_iterator Value::arr_end() const
{
  return cast<Array>()->end();
}

Struct& Value::the_struct()
{
  return *cast<Struct>();
}

const Struct& Value::the_struct() const
{
  return *cast<Struct>();
}

bool Value::has_field( const std::string& f ) const
{
  return cast<Struct>()->has_field(f);
}

const Value& Value::operator []( const std::string& s ) const
{
  return (*cast<Struct>())[s];
}

Value& Value::operator []( const std::string& s )
{
  return (*cast<Struct>())[s];
}

const Value& Value::operator []( const char* s ) const
{
  return (*cast<Struct>())[s];
}

Value& Value::operator []( const char* s )
{
  return (*cast<Struct>())[s];
}

void Value::insert( const std::string& n, const Value& v )
{
  cast<Struct>()->insert(n,v);
}

void Value::apply_visitor(Value_type_visitor& v) const
{
  v.visit_value(*value);
}

//
// Free functions
//

void value_to_xml(const Value& v, xmlpp::Node* node)
{
  Value_type_to_xml vis(node);
  v.apply_visitor(vis);
}

void print_value(const Value& v, std::ostream& s)
{
  Print_value_visitor vis(s);
  v.apply_visitor(vis);
}

} // namespace iqxmlrpc
