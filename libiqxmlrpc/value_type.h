/*! \file */
#ifndef _iqxmlrpc_value_type_h_
#define _iqxmlrpc_value_type_h_

#include <string>
#include <libxml++/libxml++.h>
#include <libiqxmlrpc/except.h>


//! XML-RPC library
namespace iqxmlrpc
{
  class Value;
  class Value_type;
  class Array;
  class Struct;

  template <class T> class Scalar;
  typedef Scalar<int> Int;
  typedef Scalar<bool> Bool;
  typedef Scalar<double> Double;
  typedef Scalar<std::string> String;
};


//! Base type for XML-RPC types.
class iqxmlrpc::Value_type {
public:
  virtual ~Value_type() {}
  
  virtual Value_type*  clone()  const = 0;
  virtual void to_xml( xmlpp::Node* parent ) const = 0;
};


//! Template for scalar types based on Value_type (e.g. Int, String, etc.)
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
class iqxmlrpc::Array: public iqxmlrpc::Value_type {
public:
  class Out_of_range: public Exception {
  public:
    Out_of_range():
      Exception( "iqxmlrpc::Array: index out of range." ) {}
  };

private:
  typedef std::vector<Value*>::const_iterator const_iterator;
  typedef std::vector<Value*>::iterator iterator;

  std::vector<Value*> values;
  
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

private:
  Array( const Array& ) {}
  const Array& operator =( const Array& ) { return *this; }
};


//! XML-RPC array type. Operates with objects of type Value, not Value_type.
class iqxmlrpc::Struct: public iqxmlrpc::Value_type {
public:
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

  void insert( const std::string&, Value* );
  void insert( const std::string&, const Value& );

private:
  Struct( const Struct& ) {}
  const Struct& operator =( const Struct& ) { return *this; }
};


#endif
