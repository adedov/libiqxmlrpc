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
/*! \see \ref value_types */
class iqxmlrpc::Value_type {
public:
  virtual ~Value_type() {}
  
  virtual Value_type*  clone()  const = 0;
  virtual void to_xml( xmlpp::Node* parent ) const = 0;
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
public:
  //! Exception which is being thrown on array range violation.
  class Out_of_range: public Exception {
    enum { code = Fault_code::xmlrpc_usage };
    
  public:
    Out_of_range():
      Exception( "iqxmlrpc::Array: index out of range.", code ) {}
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
  
  void clear();
  
  template <class In>
  void assign( In first, In last )
  {
    clear();
    for( In i = first; i != last; ++i )
      values.push_back( new Value(*i) );
  }

private:
  Array( const Array& ) {}
  const Array& operator =( const Array& ) { return *this; }
};


//! XML-RPC array type. Operates with objects of type Value, not Value_type.
/*! \see \ref struct_usage */
class iqxmlrpc::Struct: public iqxmlrpc::Value_type {
public:
  //! Exception which is being thrown when user tries 
  //! to access structure's unexistent member.
  class No_field: public Exception {
    enum { code = Fault_code::xmlrpc_usage };
    
  public:
    No_field( const std::string& f ):
      Exception( "iqxmlrpc::Struct: field '" + f + "' not exist.", code ) {}
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


#endif
