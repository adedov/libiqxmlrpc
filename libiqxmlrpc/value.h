#ifndef _iqxmlrpc_value_h_
#define _iqxmlrpc_value_h_

#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <libiqxmlrpc/value_type.h>
#include <libiqxmlrpc/except.h>


namespace iqxmlrpc 
{
  class Value;
};


//! Proxy class to access XML-RPC values by users.
/*! For more documentation please look into \ref value_usage .
    \exception Bad_cast 
*/
class iqxmlrpc::Value {
public:
  //! Bad_cast is being thrown on illegal 
  //! type conversion or Value::get_X() call.
  class Bad_cast: public Exception {
    enum { code = Fault_code::xmlrpc_usage };
    
  public:
    Bad_cast():
      Exception( "iqxmlrpc::Value: incorrect type was requested.", code ) {}
  };
  
private:
  Value_type* value;
  
public:
  Value( Value_type* );
  Value( const Value& );
  Value( int );
  Value( bool );
  Value( double );
  Value( std::string );
  Value( const char* );
  Value( const Array& );
  Value( const Struct& );

  virtual ~Value();

  const Value& operator =( const Value& );

  //! \name Type identification
  //! \{
  bool is_int()    const { return can_cast<Int>(); }
  bool is_bool()   const { return can_cast<Bool>(); }
  bool is_double() const { return can_cast<Double>(); }
  bool is_string() const { return can_cast<String>(); }
  bool is_array()  const { return can_cast<Array>(); }
  bool is_struct() const { return can_cast<Struct>(); }

  std::string type_debug() const { return typeid(*value).name(); }
  //! \}

  //! \name Access scalar value
  //! \{
  int         get_int()    const { return cast<Int>()->value(); }
  bool        get_bool()   const { return cast<Bool>()->value(); }
  double      get_double() const { return cast<Double>()->value(); }
  std::string get_string() const { return cast<String>()->value(); }

  operator int()         const { return get_int(); }
  operator bool()        const { return get_bool(); }
  operator double()      const { return get_double(); }
  operator std::string() const { return get_string(); }
  //! \}
  
  //! \name Array functions
  //! \{
  //! Access inner Array value
  Array& the_array() { return *cast<Array>(); }
  
  unsigned size() const { return cast<Array>()->size(); }
  const Value& operator []( int ) const;
  Value&       operator []( int );
  
  void push_back( Value* v )       { cast<Array>()->push_back(v); }
  void push_back( const Value& v ) { cast<Array>()->push_back(v); }
  //! \}
  
  //! \name Struct functions
  //! \{
  //! Access inner Struct value.
  Struct& the_struct() { return *cast<Struct>(); }

  bool has_field( const std::string& f ) const
  {
    return cast<Struct>()->has_field(f);
  }
  
  const Value& operator []( const char* ) const;
  Value&       operator []( const char* );
  const Value& operator []( const std::string& ) const;
  Value&       operator []( const std::string& );
  
  void insert( const std::string& n, Value* v )       
  { 
    cast<Struct>()->insert(n,v); 
  }
  
  void insert( const std::string& n, const Value& v ) 
  { 
    cast<Struct>()->insert(n,v); 
  }
  //! \}
  
  //! \name XML building
  //! \{
  /*!
    \param node  Parent node to attach to.
    \param debug Not enclose actual value in <value> tag.
  */
  void to_xml( xmlpp::Node* node, bool debug = false ) const;
  //! \}
  
private:
  template <class T> T* cast() const
  {
    T* t = dynamic_cast<T*>( value );
    if( !t )
      throw Bad_cast();
    return t;
  }
  
  template <class T> bool can_cast() const 
  {
    return dynamic_cast<T*>( value );
  }
};


/*! 
\page value_usage Manipuling the values
\section value_class iqxmlrpc::Value class
Users of the library should use objects of class iqxmlrpc::Value 
to store/transfer their values. Value objects can contain values of various 
types defined in libiqxmlrpc and represent an interface for 
retrieving/storing value of particular type as well as special methods which
specific only for iqxmlrpc::Array and iqxmlrpc::Struct.

\see \ref value_types

\section value_create Creating a value
One can assign Value object with following type of argument:
  - int
  - bool
  - double
  - std::string
  - const char*
  - iqxmlrpc::Array
  - iqxmlrpc::Struct
  
In all cases new object of class inherited from iqxmlrpc::Value_type will be
created.

\b Example:
\code
  Value v1 = 10;
  Value v2 = true;
  Value v3 = 0.44;
  Value v4 = "C string";
  Value v5 = Array();
  Value v6 = Struct();
  
  v5.push_back( true );
  v6.insert( "test_item", 0.33 );
\endcode

\section value_convert Accessing values
One can convert Value to a concrete type:
\code
  int          i = v1;
  bool         b = v2;
  double       d = v3;
  std::string  s = v4;
  bool        b2 = v5[0];
  double      d2 = v6["test_item"];
\endcode

When many ambiguous conversions are possible use Value::get_xxx() functions:
\code
  try 
  {
    std::cout 
      << v1.get_int()     << std::endl
      << v2.get_bool()    << std::endl
      << v3.get_double()  << std::endl
      << v4.get_string()  << std::endl
      << v5[0].get_bool() << std::endl
      << v6["test_item"].get_double << std::endl
  }
  catch( const iqxmlrpc::Value::Bad_cast& e )
  {
    std::cerr << e.what() << std::endl;
  }
\endcode

As you can see in listing above, if user will try to preform incorrect type
conversion then exception iqxmlrpc::Value::Bad_cast would be thrown.

It is also possible to check a correctness of type explicitly:
\code
  if( v1.is_int() )
    std::cout << v1.get_int() << std::endl;
  else
    std::cerr << "int is expected." << std::endl;
\endcode

\section value_access_array Accessing Arrays and Structs
iqxmlrpc::Value provides several methods to access a Value as it would be
Array or Struct. These methods are mostly have the same name as real ones.
E.g. Value::push_back tries to call Array::push_back and so on. So one can
call some (not all) array or struct specific methods directly from Value:
\code
  Value v = Array();
  v.push_back( false );
  v.push_back( Struct() );
  v[1].insert( "test_item", 0.33 );
\endcode

One can also get access to actual array or struct by calling 
iqxmlrpc::Value::the_array() or iqxmlrpc::Value::the_struct().

\b Example:
\code
  Value v = Struct();
  // ...
  v.the_struct().clear();
\endcode
*/

#endif
