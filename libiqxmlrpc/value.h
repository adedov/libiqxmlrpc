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

\section Value class
Users of the library should use Value objects to store their values. 
Value can contain values of various types defined in libiqxmlrpc and 
represents an interface for retrieving/storing value of particular type
as well as special functions specific only for Arrays and Structs.

\subsection XML-RPC types

\section Creating value
\section Scalars
\section Arrays
\section Structs
*/

#endif
