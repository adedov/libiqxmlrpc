//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
//  
//  $Id: parser_specific.cc,v 1.7 2004-09-19 17:33:47 adedov Exp $

#include <iostream>
#include <sstream>
#include "parser_specific.h"
#include "value_type.h"
#include "value.h"
#include "except.h"

using namespace xmlpp;
using namespace iqxmlrpc;


Value_type* Nil_parser::parse_value( const Node* node ) const
{
  // TODO: check for empty tag
  return new Nil();
}


Value_type* Int_parser::parse_value( const Node* node ) const
{  
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw XML_RPC_violation::at_node(node);
    
  const TextNode *text = el->get_child_text();
  if( text )
  {
    std::stringstream ss( text->get_content() );
    int i = 0;
    ss >> i;
    
    if( !ss || !ss.eof() )
      throw XML_RPC_violation::caused( "bad int representation", node );
    
    Value_type *v = new Int( i );
    return v;
  }
  
  throw XML_RPC_violation::at_node(node);
}


Value_type* String_parser::parse_value( const Node* node ) const
{
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw XML_RPC_violation::at_node(node);
    
  const TextNode *text = el->get_child_text();
  
  if( text )
    return new String( text->get_content() );
  else
    return new String( std::string() );
}


Value_type* Boolean_parser::parse_value( const Node* node ) const
{
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw XML_RPC_violation::at_node(node);
    
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
      throw XML_RPC_violation::caused( "bad bool representation", node );
    
    Value_type *v = new Bool( b );
    return v;
  }
  
  throw XML_RPC_violation::at_node(node);
}


Value_type* Double_parser::parse_value( const Node* node ) const
{
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw XML_RPC_violation::at_node(node);
    
  const TextNode *text = el->get_child_text();
  
  if( text )
  {
    std::stringstream ss( text->get_content() );
    double d = 0;
    ss >> d;
    
    if( !ss || !ss.eof() )
      throw XML_RPC_violation::caused( "bad double representation", node );
    
    Value_type *v = new Double( d );
    return v;
  }
  
  throw XML_RPC_violation::at_node(node);
}


Value_type* Base64_parser::parse_value( const Node* node ) const
{
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw XML_RPC_violation::at_node(node);
    
  const TextNode *text = el->get_child_text();
  
  if( text )
    return Binary_data::from_base64( text->get_content() );
  else
    return Binary_data::from_base64( std::string() );
}


Value_type* Date_time_parser::parse_value( const Node* node ) const
{
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw XML_RPC_violation::at_node(node);
    
  const TextNode *text = el->get_child_text();
  
  if( text )
    return new Date_time( text->get_content() );
  else
    return new Date_time( "" );
}


Value_type* Array_parser::parse_value( const Node* node ) const
{
  Node* data = Parser::instance()->single_element( node );
  if( data->get_name() != "data" )
    throw XML_RPC_violation::at_node(node);
  
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
      throw XML_RPC_violation::at_node(*i);
    
    Node::NodeList mbr = Parser::instance()->elements_only( *i );
    if( mbr.front()->get_name() != "name" )
      throw XML_RPC_violation::at_node(mbr.front());
    
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
    throw XML_RPC_violation::at_node(node);
  
  const Element *el = dynamic_cast<const Element*>(node);
  if( !el )
    throw XML_RPC_violation::at_node(node);
  
  std::string name = el->get_child_text()->get_content();
  if( name.find_first_not_of( "\t\n\r " ) == std::string::npos )
    throw XML_RPC_violation::at_node(el);
  
  return name;
}
