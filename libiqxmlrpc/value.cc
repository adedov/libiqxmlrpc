#include <stdexcept>
#include <iostream>
#include <libiqxmlrpc/value.h>
#include <libiqxmlrpc/value_type.h>

using namespace iqxmlrpc;


Value::Value( Value_type* v ):
  value(v)
{
}


Value::Value( const Value& v ):
  value( v.value->clone() )
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


Value::~Value()
{
  delete value;
}


const Value& Value::operator =( const Value& v )
{
  delete value;
  value = v.value->clone();
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


void Value::to_xml( xmlpp::Node* p, bool debug ) const
{
  if( debug )
  {
    value->to_xml( p );
    return;
  }

  xmlpp::Element* el = p->add_child( "value" );
  value->to_xml( el );
}
