//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
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
//  $Id: value.cc,v 1.9 2006-08-30 18:01:36 adedov Exp $

#include <stdexcept>
#include "value.h"
#include "value_type_visitor.h"

namespace iqxmlrpc {

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


const Value& Value::operator =( const Value& v )
{
  Value_type* tmp = v.value->clone();
  delete value;
  value = tmp;
  return *this;
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


void Value::apply_visitor(Value_type_visitor& v) const
{
  v.visit_value(*value);
}

} // namespace iqxmlrpc
