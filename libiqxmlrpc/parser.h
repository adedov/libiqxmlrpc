#ifndef _iqxmlrpc_parser_h_
#define _iqxmlrpc_parser_h_

#include <vector>
#include <string>
#include <libxml++/libxml++.h>

namespace iqxmlrpc 
{
  class Value;
  class Value_type;
  class Value_parser;
  class Parser;
};


//! Base class for classes responsible for parsing XML-RPC values.
class iqxmlrpc::Value_parser {
public:
  virtual ~Value_parser() {}
  virtual Value_type* parse_value( const xmlpp::Node* ) const = 0;
};


//! Main parser class.
class iqxmlrpc::Parser {
  struct Type_desc 
  {
    std::string   type_name;
    std::string   xmlrpc_name;
    Value_parser *parser;
    
    template <class T> 
    static Type_desc create( const std::string& xn, Value_parser* pr )
    {
      Type_desc td( xn, pr );
      td.type_name = typeid(T).name();
      return td;
    }
    
  private:
    Type_desc( const std::string& xn, Value_parser* pr ):
      xmlrpc_name(xn), parser(pr) {}
  };
  
  typedef std::vector< Type_desc > Types_list;
  
  static Types_list types;
  static Parser* instance_;
  
public:
  static Parser* instance();
  ~Parser();

  Value* parse_value( const xmlpp::Node* );

  //! Registers Value_parser for specified type.
  template <class T>
  void register_parser( const std::string& xmlrpc_type_name, Value_parser* p )
  {
    types.push_back( Type_desc::create<T>( xmlrpc_type_name, p ) );
  }

  //! \name Various parsers interface
  //! \{
  xmlpp::Node::NodeList elements_only( const xmlpp::Node* );
  xmlpp::Element* single_element( const xmlpp::Node* );
  //! \}
  
private:
  Parser();

  void get_value_node( const xmlpp::Node*, xmlpp::Node*&, std::string& );
};

#endif
