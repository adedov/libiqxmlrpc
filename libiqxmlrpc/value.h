//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_value_h_
#define _iqxmlrpc_value_h_

#include <iosfwd>
#include <string>
#include <typeinfo>
#include <vector>

#include "except.h"
#include "value_type.h"

namespace iqxmlrpc {

//! Proxy class to access XML-RPC values by library users.
/*! \exception Bad_cast */
class LIBIQXMLRPC_API Value {
public:
  //! Bad_cast is being thrown on illegal
  //! type conversion or Value::get_X() call.
  class Bad_cast: public Exception {
  public:
    Bad_cast();
  };

private:
  Value_type* value;

public:
  Value( Value_type* );
  Value( const Value& );
  Value( Nil );
  Value( int );
  Value( bool );
  Value( double );
  Value( std::string );
  Value( const char* );
  Value( const Binary_data& );
  Value( const Date_time& );
  Value( const struct tm* );
  Value( const Array& );
  Value( const Struct& );

  virtual ~Value();

  const Value& operator =( const Value& );

  //! \name Type identification
  //! \{
  bool is_nil()    const;
  bool is_int()    const;
  bool is_bool()   const;
  bool is_double() const;
  bool is_string() const;
  bool is_binary() const;
  bool is_datetime() const;
  bool is_array()  const;
  bool is_struct() const;

  const std::string& type_name() const;
  //! \}

  //! \name Access scalar value
  //! \{
  int         get_int()    const;
  bool        get_bool()   const;
  double      get_double() const;
  std::string get_string() const;
  Binary_data get_binary() const;
  Date_time   get_datetime() const;

  operator int()         const;
  operator bool()        const;
  operator double()      const;
  operator std::string() const;
  operator Binary_data() const;
  operator struct tm()   const;
  //! \}

  //! \name Array functions
  //! \{
  //! Access inner Array value
  Array& the_array();
  const Array& the_array() const;

  unsigned size() const;
  const Value& operator []( int ) const;
  Value&       operator []( int );

  void push_back( const Value& v );

  Array::const_iterator arr_begin() const;
  Array::const_iterator arr_end() const;
  //! \}

  //! \name Struct functions
  //! \{
  //! Access inner Struct value.
  Struct& the_struct();
  const Struct& the_struct() const;

  bool has_field( const std::string& f ) const;

  const Value& operator []( const char* ) const;
  Value&       operator []( const char* );
  const Value& operator []( const std::string& ) const;
  Value&       operator []( const std::string& );

  void insert( const std::string& n, const Value& v );
  //! \}

  void apply_visitor(Value_type_visitor&) const;

private:
  template <class T> T* cast() const;
  template <class T> bool can_cast() const;
};

class XmlBuilder;
void LIBIQXMLRPC_API value_to_xml(XmlBuilder&, const Value&);
void LIBIQXMLRPC_API print_value(const Value&, std::ostream&);

} // namespace iqxmlrpc

#endif
