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
//  $Id: value_type.h,v 1.14 2004-08-02 05:04:12 adedov Exp $

/*! \file */
#ifndef _iqxmlrpc_value_type_h_
#define _iqxmlrpc_value_type_h_

#include <time.h>
#include <string>
#include <libxml++/libxml++.h>
#include "except.h"


//! XML-RPC library
namespace iqxmlrpc
{
  class Value;
  class Value_type;
  class Nil;
  class Array;
  class Struct;

  template <class T> class Scalar;
  typedef Scalar<int> Int;
  typedef Scalar<bool> Bool;
  typedef Scalar<double> Double;
  typedef Scalar<std::string> String;
  
  class Binary_data;
  class Date_time;
};


//! Base type for XML-RPC types.
/*! \see \ref value_types */
class iqxmlrpc::Value_type {
public:
  virtual ~Value_type() {}
  
  virtual Value_type*  clone()  const = 0;
  virtual void to_xml( xmlpp::Node* parent ) const = 0;
};


//! XML-RPC extension: Nil type.
/*! \see http://ontosys.com/xml-rpc/extensions.html */
class iqxmlrpc::Nil: public iqxmlrpc::Value_type {
 int nope;

public:
  Value_type* clone() const;
  void to_xml( xmlpp::Node* ) const;
};


//! Template for scalar types based on Value_type (e.g. Int, String, etc.)
/*! \see \ref value_types */
template <class T> 
class iqxmlrpc::Scalar: public iqxmlrpc::Value_type {
  T value_;
  
public:
  Scalar( T t ): value_(t) {}
  Scalar<T>* clone() const { return new Scalar<T>(value_); }

  void to_xml( xmlpp::Node* ) const;

  const T& value() const { return value_; }
  T&       value()       { return value_; }
};


//! XML-RPC array type. Operates with objects of type Value, not Value_type.
/*! \see \ref array_usage */
class iqxmlrpc::Array: public iqxmlrpc::Value_type {
  typedef std::vector<Value*> Val_vector;
  typedef Val_vector::iterator iterator;

public:
  class const_iterator;
  friend class Array::const_iterator;

  //! Exception which is being thrown on array range violation.
  class Out_of_range: public Exception {
  public:
    Out_of_range():
      Exception( "iqxmlrpc::Array: index out of range." ) {}
  };

private:
  Val_vector values;
  
public:
  Array() {}
  ~Array();

  Array* clone() const;
  void to_xml( xmlpp::Node* ) const;
  
  unsigned size() const { return values.size(); }
  
  const Value& operator []( unsigned ) const;
  Value&       operator []( unsigned );
  
  void push_back( Value* );
  void push_back( const Value& );
  
  void clear();
  
  template <class In>
  void assign( In first, In last )
  {
    clear();
    for( In i = first; i != last; ++i )
      values.push_back( new Value(*i) );
  }

  const_iterator begin() const;
  const_iterator end() const;

private:
  Array( const Array& ) {}
  const Array& operator =( const Array& ) { return *this; }
};


//! Const interator for iqxmlrpc::Array
class iqxmlrpc::Array::const_iterator {
  Array::Val_vector::const_iterator i;

public:
  const_iterator( Array::Val_vector::const_iterator i_ ):
    i(i_) {}
  ~const_iterator() {}
    
  const Value& operator *() const { return *(*i); }
  const Value* operator ->() const { return *i; }
  
  const_iterator& operator ++() { ++i; return *this; }
  const_iterator& operator --() { --i; return *this; }
  
  bool operator ==( const const_iterator& ci ) const 
  {
    return i == ci.i;
  }

  bool operator !=( const const_iterator& ci ) const 
  {
    return !(*this == ci );
  }
};


//! XML-RPC array type. Operates with objects of type Value, not Value_type.
/*! \see \ref struct_usage */
class iqxmlrpc::Struct: public iqxmlrpc::Value_type {
public:
  //! Exception which is being thrown when user tries 
  //! to access structure's unexistent member.
  class No_field: public Exception {
  public:
    No_field( const std::string& f ):
      Exception( "iqxmlrpc::Struct: field '" + f + "' not exist." ) {}
  };

private:
  typedef std::map<std::string, Value*>::const_iterator const_iterator;
  typedef std::map<std::string, Value*>::iterator iterator;

  std::map<std::string, Value*> values;
  
public:
  Struct() {}
  ~Struct();

  Struct* clone() const;
  void to_xml( xmlpp::Node* ) const;

  bool has_field( const std::string& ) const;
  
  const Value& operator []( const std::string& ) const;
  Value&       operator []( const std::string& );

  void clear();
  void insert( const std::string&, Value* );
  void insert( const std::string&, const Value& );

private:
  Struct( const Struct& ) {}
  const Struct& operator =( const Struct& ) { return *this; }
};


//! XML-RPC Base64 type.
class iqxmlrpc::Binary_data: public iqxmlrpc::Value_type {
public:
  //! Malformed base64 encoding format exception. 
  class Malformed_base64: public iqxmlrpc::Exception {
  public:
    Malformed_base64():
      Exception( "Malformed base64 format." ) {}
  };
  
private:
  static const char base64_alpha[64];

  std::string data;
  mutable std::string base64;

public:
  //! Construct an object from encoded data.
  static Binary_data* from_base64( const std::string& );
  //! Construct an object from raw data.
  static Binary_data* from_data( const std::string& );
  //! Construct an object from raw data.
  static Binary_data* from_data( const char*, unsigned size );
  
  //! Get data in encoded form.
  const std::string& get_base64() const;
  //! Get raw data.
  const std::string& get_data() const;

  Value_type* clone() const;
  void to_xml( xmlpp::Node* parent ) const;
  
private:
  class End_of_data {};

  Binary_data( const std::string&, bool raw );

  void add_base64_char( int idx ) const;
  void encode() const;

  char get_idx( char );
  void decode_four( const std::string& );
  void decode();
};


//! XML-RPC dateTime.iso8601 type.
class iqxmlrpc::Date_time: public iqxmlrpc::Value_type {
public:
  //! Malformed dateTime.iso8601 format exception. 
  class Malformed_iso8601: public iqxmlrpc::Exception {
  public:
    Malformed_iso8601():
      Exception( "Malformed date-time format." ) {}
  };
  
private:
  struct tm tm_;
  mutable std::string cache;

public:
  Date_time( const struct tm* );
  explicit Date_time( const std::string& dateTime_iso8601 );
  explicit Date_time( bool localtime );

  const struct tm& get_tm() const { return tm_; }
  const std::string& to_string() const;  
  
  Value_type* clone() const;
  void to_xml( xmlpp::Node* ) const;
};


#endif
