#include <iostream>
#include <sstream>
#include <libiqxmlrpc/parser_specific.h>
#include <libiqxmlrpc/value_type.h>
#include <libiqxmlrpc/value.h>
#include <libiqxmlrpc/except.h>

using namespace xmlpp;
using namespace iqxmlrpc;


Value_type* Int_parser::parse_value( const Node* node ) const
{  
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw Parse_error::at_node(node);
    
  const TextNode *text = el->get_child_text();
  if( text )
  {
    std::stringstream ss( text->get_content() );
    int i = 0;
    ss >> i;
    
    if( !ss || !ss.eof() )
      throw Parse_error::caused( "bad int representation", node );
    
    Value_type *v = new Int( i );
    return v;
  }
  
  throw Parse_error::at_node(node);
}


Value_type* String_parser::parse_value( const Node* node ) const
{
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw Parse_error::at_node(node);
    
  const TextNode *text = el->get_child_text();
  
  if( text )
  {
    Value_type *v = new String( text->get_content() );
    return v;
  }
  
  throw Parse_error::at_node(node);
}


Value_type* Boolean_parser::parse_value( const Node* node ) const
{
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw Parse_error::at_node(node);
    
  const TextNode *text = el->get_child_text();
  
  if( text )
  {
    std::string s( text->get_content() );
    bool b;
    if( s == "1" || s == "true" )
      b = true;
    else if( s == "0" || s == "false" )
      b = false;
    else
      throw Parse_error::caused( "bad bool representation", node );
    
    Value_type *v = new Bool( b );
    return v;
  }
  
  throw Parse_error::at_node(node);
}


Value_type* Double_parser::parse_value( const Node* node ) const
{
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw Parse_error::at_node(node);
    
  const TextNode *text = el->get_child_text();
  
  if( text )
  {
    std::stringstream ss( text->get_content() );
    double d = 0;
    ss >> d;
    
    if( !ss || !ss.eof() )
      throw Parse_error::caused( "bad double representation", node );
    
    Value_type *v = new Double( d );
    return v;
  }
  
  throw Parse_error::at_node(node);
}


Value_type* Array_parser::parse_value( const Node* node ) const
{
  Node* data = Parser::instance()->single_element( node );
  if( data->get_name() != "data" )
    throw Parse_error::at_node(node);
  
  Node::NodeList value_arr = Parser::instance()->elements_only( data );
  Array retval;
  
  typedef Node::NodeList::const_iterator CI;
  for( CI i = value_arr.begin(); i != value_arr.end(); ++i )
  {
    Value *v = Parser::instance()->parse_value(*i);
    retval.push_back( v );
  }

  return retval.clone();
}


Value_type* Struct_parser::parse_value( const Node* node ) const
{
  Node::NodeList members = Parser::instance()->elements_only( node );
  Struct s;
  
  typedef Node::NodeList::const_iterator CI;
  for( CI i = members.begin(); i != members.end(); i++ )
  {
    if( (*i)->get_name() != "member" )
      throw Parse_error::at_node(*i);
    
    Node::NodeList mbr = Parser::instance()->elements_only( *i );
    if( mbr.front()->get_name() != "name" )
      throw Parse_error::at_node(mbr.front());
    
    std::string name = get_member_name( mbr.front() );
    Value* value = Parser::instance()->parse_value( mbr.back() );
    s.insert( name, value );
  }
  
  return s.clone();
}


inline std::string Struct_parser::get_member_name( const Node* node ) const
{
  Node::NodeList nlist = node->get_children();
  if( nlist.size() != 1 )
    throw Parse_error::at_node(node);
  
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw Parse_error::at_node(node);
  
  std::string name = el->get_child_text()->get_content();
  if( name.find_first_not_of( "\t\n\r " ) == std::string::npos )
    throw Parse_error::at_node(el);
  
  return name;
}
