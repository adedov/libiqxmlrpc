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


/*!
\page value_types Data types
XML-RPC (http://www.xmlrpc.com) standard assumes following data types to use.

- \b Scalars:
  - string
  - integer (four bytes)
  - boolean
  - double
  - datetime (ISO8601)
  - base64
- \b Compond \b types:
  - array
  - struct

\todo Types datetime and base64 are not supported in libiqxmlrpc yet.

Library maps each XML-RPC type into a stand-alone class. Here is a piece of 
code which declares libiqxmlrpc data type classes (value_type.h):
\dontinclude value_type.h
\skip namespace
\skip iqxmlrpc
\until }

As you can see, all scalar type classes are just an instance of template
iqxmlrpc::Scalar.

\section array_usage Array's usage
iqxmlrpc::Array represents XML-RPC array data type. One can insert an array's
member of any data type to the end of array.

\code
  Array a;
  // insert data into array
  a.push_back( 1 );
  a.push_back( "Hello World!" );
  
  // access the value
  std::cout 
    << "a is of size: " << a.size()          << std::endl
    << "a[0] = "        << a[0].get_int()    << std::endl
    << "a[1] = "        << a[1].get_string() << std::endl;

  // STL's assign analog
  std::vector<double> v;
  // filling v...
  a.assign( v.begin(), v.end() );
\endcode

Array::push_back() takes object of iqxmlrpc::Value class as an argument.
Hence class Value has constructors which can take std::string, int, double, ... 
as an argument, one can transfer std::string or int rather then Value as 
argument for push_back().

Arrays's operator [] returns a reference to iqxmlrpc::Value class' object.
If one will try access to out of bound element of array then exception
iqxmlrpc::Array::Out_of_range will be thrown.

\see \ref value_usage 

\section struct_usage Struct's usage
iqxmlrpc::Struct represents data structures. It holds objects of 
iqxmlrpc::Value class as structure members.

\code
  Struct s;
  
  // insert data to a struct
  s.insert( "author", "Gabriel Garcia Marques" );
  s.insert( "title", "One Hundred Years of Solitude" );
  s.insert( "pages", 464 );
\endcode

Struct::insert() takes object of iqxmlrpc::Value class as second argument.
As soon as it has constructors which can take std::string, int, double, ... as
an argument, one can transfer std::string or int rather then Value as argument 
for insert().

\code
  // check if struct contains specific members
  // and access an actual value
  if( s.has_field( "price" )
    std::cout << s["price"].get_double() << std::endl;
\endcode

Struct's operator [] returns a reference to iqxmlrpc::Value class' object.
If one wont use Struct::has_field function and user will query Struct for 
unexistent member then iqxmlrpc::Struct::No_field exception would be thrown.

\see \ref value_usage
*/

#endif
