#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <libiqxmlrpc/parser.h>
#include <libiqxmlrpc/except.h>
#include <libiqxmlrpc/parser_specific.h>
#include <libiqxmlrpc/value_type.h>
#include <libiqxmlrpc/value.h>

using namespace iqxmlrpc;


Parser::Types_list Parser::types;
Parser* Parser::instance_ = 0;


Parser* Parser::instance()
{
  if( !Parser::instance_ )
    Parser::instance_ = new Parser;
  
  return Parser::instance_;
}


Parser::Parser()
{
  if( types.empty() )
  {
    Value_parser* i4p = new Int_parser;
    register_parser<int>( "i4", i4p );
    register_parser<int>( "int", i4p );
    register_parser<bool>( "boolean", new Boolean_parser );
    register_parser<double>( "double", new Double_parser );
    register_parser<std::string>( "string", new String_parser );
    register_parser<std::string>( "", new String_parser ); // default type
    register_parser<Array>( "array", new Array_parser );
    register_parser<Struct>( "struct", new Struct_parser );
  }
}


Parser::~Parser()
{
  instance_ = 0;
}


Value* Parser::parse_value( const xmlpp::Node* node )
{
  using namespace xmlpp;
  
  if( node->get_name() != "value" )
    throw Parse_error::at_node( node );
  
  Node *valnode = 0;
  std::string tname;
  get_value_node( node, valnode, tname );
  
  for( Types_list::const_iterator i = types.begin(); i != types.end(); ++i )
  {
    if( i->xmlrpc_name == tname )
      return new Value( i->parser->parse_value(valnode) );
  }
  
  throw Parse_error::caused( "unknown XML-RPC value type '" + tname + "'" );
}


xmlpp::Node::NodeList Parser::elements_only( const xmlpp::Node* node )
{
  using namespace xmlpp;
  
  Node::NodeList childs = node->get_children();
  Node::NodeList elems;
  
  typedef Node::NodeList::const_iterator CI;
  for( CI i = childs.begin(); i != childs.end(); ++i )
  {
    const TextNode* text = dynamic_cast<const TextNode*>(*i);
    if( text )
    {
      std::string s( text->get_content() );
      if( s.find_first_not_of("\t\n\r ") != std::string::npos )
        throw Parse_error::at_node(*i);
      else
        continue;
    }
    
    Element* el = dynamic_cast<Element*>(*i);
    if( el )
      elems.push_back( el );
  }

  return elems;
}


xmlpp::Element* Parser::single_element( const xmlpp::Node* node )
{
  xmlpp::Node::NodeList lst = elements_only( node );
  if( lst.size() != 1 )
    throw Parse_error::at_node(node);
  
  return dynamic_cast<xmlpp::Element*>(lst.front());
}


void Parser::get_value_node( 
  const xmlpp::Node* node, xmlpp::Node*& valnode, std::string& type )
{
  using namespace xmlpp;

  Node::NodeList childs = node->get_children();
  if( childs.size() == 1 )
  {
    // Check for default node type
    TextNode* text = dynamic_cast<TextNode*>(childs.front());
    if( text )
    {
      valnode = const_cast<Node*>(node);
      type = "";
      return;
    }
  }
  
  valnode = single_element(node);
  type = valnode->get_name();
}
