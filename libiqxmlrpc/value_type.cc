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
//  $Id: value_type.cc,v 1.21 2005-06-06 17:03:01 bada Exp $

#include <string.h>
#include <algorithm>
#include "value_type.h"
#include "value.h"
#include "utf_conv.h"
#include "util.h"

using namespace iqxmlrpc;


Value_type* Nil::clone() const
{
  return new Nil();
}


void Nil::to_xml( xmlpp::Node* p ) const
{
  p->add_child( "nil" );
}


// --------------------- Scalar's specialization ------------------------------
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


// --------------------------------------------------------------------------
void iqxmlrpc::String::to_xml( xmlpp::Node* p ) const
{
  xmlpp::Element* el = p->add_child( "string" );
  el->add_child_text( config::cs_conv->to_utf(value_) );
}


// --------------------------------------------------------------------------
#ifndef DOXYGEN_SHOULD_SKIP_THIS
class Array::Array_inserter: public std::unary_function<Value,void> {
  Array::Val_vector* vv;
  
public:
  Array_inserter( Array::Val_vector* v ): vv(v) {}
    
  void operator ()( const Value& v )
  {
    vv->push_back( new Value(v) );
  }
};
#endif


Array::Array( const Array& other )
{
  std::for_each( other.begin(), other.end(), Array_inserter(&values) );
}


Array::~Array()
{
  clear();
}


Array& Array::operator =( const Array& other )
{
  if( this == &other )
    return *this;

  Array tmp(other);
  tmp.swap(*this);
  return *this;
}


void Array::swap( Array& other) throw()
{
  values.swap(other.values);
}


Array* Array::clone() const
{
  return new Array(*this);
}


void Array::to_xml( xmlpp::Node* p ) const
{
  xmlpp::Element* arr_el = p->add_child( "array" );
  xmlpp::Element* el = arr_el->add_child( "data" );
  unsigned sz = size();
  
  for( unsigned i = 0; i < sz; i++ )
    (*this)[i].to_xml( el );
}


void Array::clear()
{
  util::delete_ptrs(values.begin(), values.end());
  
  // Clear and free memory
  std::vector<Value*>().swap( values );
}


// This member placed here because of mutual dependence of 
// value_type.h and value.h
void Array::push_back( Value_ptr v )
{ 
  values.push_back(v.release());
}


// This member placed here because of mutual dependence of 
// value_type.h and value.h
void Array::push_back( const Value& v ) 
{ 
  values.push_back(new Value(v)); 
}


// --------------------------------------------------------------------------
#ifndef DOXYGEN_SHOULD_SKIP_THIS
class Struct::Struct_inserter: 
  public std::unary_function<std::pair<std::string, Value*>,void> 
{
  Struct::Value_stor* vs;
  
public:
  Struct_inserter( Struct::Value_stor* v ): vs(v) {}
    
  void operator ()( const std::pair<std::string, Value*>& vp )
  {
    vs->insert( std::make_pair(vp.first, new Value(*vp.second)) );
  }
};
#endif


Struct::Struct( const Struct& other )
{
  std::for_each( other.begin(), other.end(), Struct_inserter(&values) );
}


Struct& Struct::operator =( const Struct& other )
{
  if( this == &other )
    return *this;

  Struct tmp(other);
  tmp.swap(*this);
  return *this;
}


Struct::~Struct()
{
  clear();
}


void Struct::swap( Struct& other ) throw()
{
  values.swap(other.values);
}


Struct* Struct::clone() const
{
  return new Struct(*this);
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
  util::delete_ptrs(values.begin(), values.end(), 
    util::Select2nd<Value_stor>());
  
  values.clear();
}


void Struct::insert( const std::string& f, Value_ptr val )
{
  values[f] = val.release();
}


void Struct::insert( const std::string& f, const Value& val )
{
  values[f] = new Value(val);
}


// ----------------------------------------------------------------------------
const char Binary_data::base64_alpha[64] = { 
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };


Binary_data* Binary_data::from_base64( const std::string& s )
{
  return new Binary_data( s, false );
}


Binary_data* Binary_data::from_data( const std::string& s )
{
  return new Binary_data( s, true );
}


Binary_data* Binary_data::from_data( const char* s, unsigned size )
{
  return new Binary_data( std::string(s, size), true );
}


Binary_data::Binary_data( const std::string& s, bool raw )
{
  if( raw )
    data = s;
  else
  {
    base64 = s;
    decode();
  }
}


const std::string& Binary_data::get_data() const 
{
  return data;
}


const std::string& Binary_data::get_base64() const 
{
  if( base64.empty() && !data.empty() )
    encode();

  return base64;
}


inline void Binary_data::add_base64_char( int idx ) const
{
  if( !(base64.length() % chars_in_line) )
    base64 += '\n';
    
  base64 += base64_alpha[idx];
}


void Binary_data::encode() const 
{
  const char* d = data.data();
  unsigned dsz = data.length();

  for( unsigned i = 0; i < dsz; i += 3 )
  {
    unsigned c = 0xff0000 & d[i] << 16;
    add_base64_char( (c >> 18) & 0x3f );

    if( i+1 < dsz )
    {
      c |= 0x00ff00 & d[i+1] << 8;
      add_base64_char( (c >> 12) & 0x3f );
    }
    else
    {
      add_base64_char( (c >> 12) & 0x3f );
      base64 += "==";
      return;
    }
    
    if( i+2 < dsz )
    {
      c |= 0x0000ff & d[i+2];
      add_base64_char( (c >> 6) & 0x3f );
      add_base64_char( c & 0x3f );
    }
    else
    {
      add_base64_char( (c >> 6) & 0x3f );
      base64 += "=";
      return;
    }
  }
}


inline char Binary_data::get_idx( char c )
{
  if( c == '=' )
    throw End_of_data();

  if( c >= 'A' && c <= 'Z' )
    return c - 'A';
  
  if( c >= 'a' && c <= 'z' )
    return 26 + c - 'a';
  
  if( c >= '0' && c <= '9' )
    return 52 + c - '0';
  
  if( c == '+' )
    return 62;
  
  if( c == '/' )
    return 63;

  throw Malformed_base64();  
}


inline void Binary_data::decode_four( const std::string& four )
{
  char c1 = four[0];
  char c2 = four[1];
  char c3 = four[2];
  char c4 = four[3];
  
  if( c1 == '=' || c2 == '=' )
    throw Malformed_base64();
  
  try {
    unsigned pair = get_idx(c1) << 6 | get_idx(c2);
    data += char(pair >> 4 & 0xff);
  
    pair = get_idx(c2) << 6 | get_idx(c3);
    data += char(pair >> 2);
  
    pair = get_idx(c3) << 6 | get_idx(c4);
    data += char(pair & 0xff);
  }
  catch( const End_of_data& )
  {
  }
}


void Binary_data::decode()
{
  const char* d = base64.data();
  int dsz = base64.length();
  std::string four;
  
  for( int i = 0; i < dsz; i++ )
  {
    if( isspace( d[i] ) )
      continue;
    
    four += d[i];
    if( four.length() == 4 )
    {
      decode_four( four );
      four.erase();
    }
  }
  
  if( !four.empty() )
    throw Malformed_base64();
}


Value_type* Binary_data::clone() const 
{
  return new Binary_data(*this);
}


void Binary_data::to_xml( xmlpp::Node* p ) const 
{
  xmlpp::Element* el = p->add_child( "base64" );
  el->add_child_text( get_base64() );
}


// ----------------------------------------------------------------------------
Date_time::Date_time( const struct tm* t )
{
  tm_ = *t;
}


Date_time::Date_time( bool use_lt )
{
  time_t t;
  ::time( &t );
  struct tm *ptm = use_lt ? localtime( &t ) : gmtime( &t );
  tm_ = *ptm;
}


Date_time::Date_time( const std::string& s )
{
  if( s.length() != 17 || s[8] != 'T' )
    throw Malformed_iso8601();
  
  char alpha[] = "0123456789T:";
  if( s.substr(0, 16).find_first_not_of(alpha) != std::string::npos )
    throw Malformed_iso8601();

  tm_.tm_year = atoi( s.substr(0, 4).c_str() ) - 1900;
  tm_.tm_mon  = atoi( s.substr(4, 2).c_str() ) - 1;
  tm_.tm_mday = atoi( s.substr(6, 2).c_str() );
  tm_.tm_hour = atoi( s.substr(9, 2).c_str() );
  tm_.tm_min  = atoi( s.substr(12, 2).c_str() );
  tm_.tm_sec  = atoi( s.substr(15, 2).c_str() );
  
  if( (tm_.tm_year < 0) || !(tm_.tm_mon >= 0 && tm_.tm_mon <= 11) ||
      !(tm_.tm_mday >= 1 && tm_.tm_mday <= 31) ||
      !(tm_.tm_hour >= 0 && tm_.tm_hour <= 23) ||
      !(tm_.tm_min >= 0 && tm_.tm_min <= 59) ||
      !(tm_.tm_sec >= 0 && tm_.tm_sec <= 61) 
    )
    throw Malformed_iso8601();
}


Value_type* Date_time::clone() const
{
  return new Date_time( *this );
}


void Date_time::to_xml( xmlpp::Node* p ) const
{
  xmlpp::Element* el = p->add_child( "dateTime.iso8601" );
  el->add_child_text( to_string() );  
}


const std::string& Date_time::to_string() const
{
  if( cache.empty() )
  {
    char s[18];
    strftime( s, 18, "%Y%m%dT%H:%M:%S", &tm_ );
    cache = std::string( s, 17 );
  }
  
  return cache;
}
