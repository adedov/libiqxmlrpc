//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

/*! \file */
#ifndef _iqxmlrpc_value_type_h_
#define _iqxmlrpc_value_type_h_

#include "except.h"
#include "util.h"

#include <iterator>
#include <map>
#include <string>
#include <time.h>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4275)
#endif

//! XML-RPC library
namespace iqxmlrpc {

class Value;
class Value_type_visitor;
typedef util::ExplicitPtr<Value*> Value_ptr;

template <class T> class Scalar;
typedef Scalar<int> Int;
typedef Scalar<int64_t> Int64;
typedef Scalar<bool> Bool;
typedef Scalar<double> Double;
typedef Scalar<std::string> String;


//! Base type for XML-RPC types.
class LIBIQXMLRPC_API Value_type {
public:
  virtual ~Value_type() {}

  virtual Value_type*  clone()  const = 0;
  virtual const std::string& type_name() const = 0;
  virtual void apply_visitor(Value_type_visitor&) const = 0;
};


//! XML-RPC extension: Nil type.
/*! \see http://ontosys.com/xml-rpc/extensions.html */
class LIBIQXMLRPC_API Nil: public Value_type {
  int nope;

public:
  Value_type* clone() const;
  const std::string& type_name() const;
  void apply_visitor(Value_type_visitor&) const;
};


//! Template for scalar types based on Value_type (e.g. Int, String, etc.)
template <class T>
class LIBIQXMLRPC_API Scalar: public Value_type {
protected:
  T value_;

public:
  Scalar( const T& t ): value_(t) {}
  Scalar<T>* clone() const { return new Scalar<T>(value_); }

  void apply_visitor(Value_type_visitor&) const;
  const std::string& type_name() const;

  const T& value() const { return value_; }
  T&       value()       { return value_; }
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

//! XML-RPC array type. Operates with objects of type Value, not Value_type.
class LIBIQXMLRPC_API Array: public Value_type {
  typedef std::vector<Value*> Val_vector;
  typedef Val_vector::iterator iterator;

  class Array_inserter;
  friend class Array_inserter;

public:
  typedef Value value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;

  class const_iterator;
  friend class Array::const_iterator;

  //! Exception which is being thrown on array range violation.
  class Out_of_range: public Exception {
  public:
    Out_of_range():
      Exception( "Array: index out of range." ) {}
  };

private:
  Val_vector values;

public:
  Array( const Array& );
  Array() {}
  ~Array();

  Array& operator =( const Array& );

  void swap(Array&) throw();
  Array* clone() const;
  const std::string& type_name() const;
  void apply_visitor(Value_type_visitor&) const;

  size_t size() const { return values.size(); }

  const Value& operator []( unsigned i ) const
  {
    try {
      return (*values.at(i));
    }
    catch( const std::out_of_range& )
    {
      throw Out_of_range();
    }
  }

  Value& operator []( unsigned i )
  {
    try {
      return (*values.at(i));
    }
    catch( const std::out_of_range& )
    {
      throw Out_of_range();
    }
  }

  void push_back( const Value& );
  void push_back( Value_ptr );

  void clear();

  //! Clears array and assigns from specified container's interval.
  template <class In>
  void assign( In first, In last );

  Array::const_iterator begin() const;
  Array::const_iterator end()   const;
};


//! Const interator for Array
class LIBIQXMLRPC_API Array::const_iterator:
  public std::iterator<std::bidirectional_iterator_tag, Value>
{
  Array::Val_vector::const_iterator i;

public:
  const_iterator( Array::Val_vector::const_iterator i_ ):
    i(i_) {}
  ~const_iterator() {}

  const Value& operator *() const { return *(*i); }
  const Value* operator ->() const { return *i; }

  const_iterator operator ++( int ) { return const_iterator(i++); }
  const_iterator operator --( int ) { return const_iterator(i--); }

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

inline Array::const_iterator Array::begin() const
{
  return values.begin();
}


inline Array::const_iterator Array::end() const
{
  return values.end();
}


//! XML-RPC array type. Operates with objects of type Value, not Value_type.
class LIBIQXMLRPC_API Struct: public Value_type {
public:
  //! Exception which is being thrown when user tries
  //! to access structure's unexistent member.
  class No_field: public Exception {
  public:
    No_field( const std::string& f ):
      Exception( "Struct: field '" + f + "' not exist." ) {}
  };

private:
  typedef std::map<std::string, Value*> Value_stor;
  class Struct_inserter;
  friend class Struct_inserter;

  Value_stor values;

public:
  typedef Value_stor::const_iterator const_iterator;
  typedef Value_stor::iterator iterator;

  Struct( const Struct& );
  Struct() {}
  ~Struct();

  Struct& operator =( const Struct& );

  void swap(Struct&) throw();
  Struct* clone() const;
  const std::string& type_name() const;
  void apply_visitor(Value_type_visitor&) const;

  size_t size() const;
  bool has_field( const std::string& ) const;

  const Value& operator []( const std::string& ) const;
  Value&       operator []( const std::string& );

  void clear();
  void insert( const std::string&, Value_ptr );
  void insert( const std::string&, const Value& );

  const_iterator begin() const { return values.begin(); }
  const_iterator end()   const { return values.end(); }

  const_iterator find( const std::string& key ) const { return values.find(key); }
  iterator find( const std::string& key ) { return values.find(key); }

  void erase( const std::string& key ) { values.erase(key); }
};

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif

//! XML-RPC Base64 type.
class LIBIQXMLRPC_API Binary_data: public Value_type {
public:
  //! Malformed base64 encoding format exception.
  class Malformed_base64: public Exception {
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
  static Binary_data* from_data( const char*, size_t size );

  //! Get data in encoded form.
  const std::string& get_base64() const;
  //! Get raw data.
  const std::string& get_data() const;

  Value_type* clone() const;
  const std::string& type_name() const;
  void apply_visitor( Value_type_visitor& ) const;

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
class LIBIQXMLRPC_API Date_time: public Value_type {
public:
  //! Malformed dateTime.iso8601 format exception.
  class Malformed_iso8601: public Exception {
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
  const std::string& type_name() const;
  void apply_visitor(Value_type_visitor&) const;
};

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
