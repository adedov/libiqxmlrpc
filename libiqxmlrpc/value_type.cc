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
//  $Id: value_type.cc,v 1.5 2004-04-14 08:56:40 adedov Exp $

#include <sstream>
#include "value_type.h"
#include "value.h"

using namespace iqxmlrpc;


void Int::to_xml( xmlpp::Node* p ) const 
{
  xmlpp::Element* el = p->add_child( "i4" );
  
  std::stringstream ss;
  ss << value_;
  el->add_child_text( ss.str() );
}


void Bool::to_xml( xmlpp::Node* p ) const
{
  xmlpp::Element* el = p->add_child( "boolean" );
  el->add_child_text( value_ ? "1" : "0" );
}


void Double::to_xml( xmlpp::Node* p ) const
{
  xmlpp::Element* el = p->add_child( "double" );
  
  std::stringstream ss;
  ss << value_;
  el->add_child_text( ss.str() );
}


void String::to_xml( xmlpp::Node* p ) const
{
  xmlpp::Element* el = p->add_child( "string" );
  el->add_child_text( value_ );
}


// --------------------------------------------------------------------------
Array* Array::clone() const
{
  Array *a = new Array;
  for( const_iterator i = values.begin(); i != values.end(); ++i )
    a->push_back( new Value(**i) );

  return a;
}


Array::~Array()
{
  clear();
}


void Array::to_xml( xmlpp::Node* p ) const
{
  xmlpp::Element* arr_el = p->add_child( "array" );
  xmlpp::Element* el = arr_el->add_child( "data" );
  unsigned sz = size();
  
  for( int i = 0; i < sz; i++ )
    (*this)[i].to_xml( el );
}


const Value& Array::operator []( unsigned i ) const
{
  try {
    return (*values.at(i));
  }
  catch( std::out_of_range )
  {
    throw Out_of_range();
  }
}


Value& Array::operator []( unsigned i )
{
  try {
    return (*values.at(i));
  }
  catch( std::out_of_range )
  {
    throw Out_of_range();
  }
}


void Array::push_back( Value* val )
{
  values.push_back( val );
}


void Array::push_back( const Value& val )
{
  values.push_back( new Value(val) );
}


void Array::clear()
{
  for( iterator i = values.begin(); i != values.end(); ++i )
    delete *i;
  
  // Clear and free memory
  std::vector<Value*>().swap( values );
}


// --------------------------------------------------------------------------
Struct::~Struct()
{
  clear();
}


Struct* Struct::clone() const
{
  Struct *s = new Struct;
  for( const_iterator i = values.begin(); i != values.end(); ++i )
    s->insert( i->first, new Value( *(i->second) ) );
  
  return s;
}


void Struct::to_xml( xmlpp::Node* p ) const
{
  xmlpp::Element* str_el = p->add_child( "struct" );
  
  for( const_iterator i=values.begin(); i != values.end(); ++i )
  {
    xmlpp::Element* mbr_el = str_el->add_child( "member" );
    xmlpp::Element* name_el = mbr_el->add_child( "name" );
    name_el->add_child_text( i->first );
    i->second->to_xml( mbr_el );
  }
}


bool Struct::has_field( const std::string& f ) const
{
  return values.find(f) != values.end();
}


const Value& Struct::operator []( const std::string& f ) const
{
  const_iterator i = values.find(f);

  if( i == values.end() )
    throw No_field( f );
  
  return (*i->second);
}


Value& Struct::operator []( const std::string& f )
{
  const_iterator i = values.find(f);

  if( i == values.end() )
    throw No_field( f );
  
  return (*i->second);
}


void Struct::clear()
{
  for( iterator i = values.begin(); i != values.end(); ++i )
    delete i->second;
  
  values.clear();
}


void Struct::insert( const std::string& f, Value* val )
{
  values[f] = val;
}


void Struct::insert( const std::string& f, const Value& val )
{
  values[f] = new Value(val);
}
