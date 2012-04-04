//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "value_type.h"

#include "util.h"
#include "value.h"
#include "value_type_visitor.h"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <algorithm>
#include <string.h>

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


Value_type* Nil::clone() const
{
  return new Nil();
}


const std::string& Nil::type_name() const
{
  return type_names::nil_type_name;
}


void Nil::apply_visitor(Value_type_visitor& v) const
{
  v.visit_nil();
}


// --------------------- Scalar's specialization ------------------------------
template<>
const std::string& Int::type_name() const
{
  return type_names::int_type_name;
}

template<>
void Int::apply_visitor(Value_type_visitor& v) const
{
  v.visit_int(value_);
}

template<>
const std::string& Bool::type_name() const
{
  return type_names::bool_type_name;
}

template<>
void Bool::apply_visitor(Value_type_visitor& v) const
{
  v.visit_bool(value_);
}

template<>
const std::string& Double::type_name() const
{
  return type_names::double_type_name;
}

template<>
void Double::apply_visitor(Value_type_visitor& v) const
{
  v.visit_double(value_);
}

template<>
const std::string& String::type_name() const
{
  return type_names::string_type_name;
}

template<>
void String::apply_visitor(Value_type_visitor& v) const
{
  v.visit_string(value_);
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


const std::string& Array::type_name() const
{
  return type_names::array_type_name;
}


void Array::apply_visitor(Value_type_visitor& v) const
{
  v.visit_array(*this);
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


const std::string& Struct::type_name() const
{
  return type_names::struct_type_name;
}


void Struct::apply_visitor(Value_type_visitor& v) const
{
  v.visit_struct(*this);
}


size_t Struct::size() const
{
  return values.size();
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
  Value*& tmp = values[f];
  delete tmp;
  tmp = val.release();
}


void Struct::insert( const std::string& f, const Value& val )
{
  Value_ptr p(new Value(val));
  insert(f, p);
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


const std::string& Binary_data::type_name() const
{
  return type_names::base64_type_name;
}


void Binary_data::apply_visitor(Value_type_visitor& v) const
{
  v.visit_base64(*this);
}


// ----------------------------------------------------------------------------
Date_time::Date_time( const struct tm* t )
{
  tm_ = *t;
}


Date_time::Date_time( bool use_lt )
{
  using namespace boost::posix_time;
  ptime p = use_lt ? second_clock::local_time() : second_clock::universal_time();
  tm_ = to_tm(p);
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


const std::string& Date_time::type_name() const
{
  return type_names::date_type_name;
}


void Date_time::apply_visitor(Value_type_visitor& v) const
{
  v.visit_datetime(*this);
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

} // namespace iqxmlrpc
