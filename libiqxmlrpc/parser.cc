//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2006 Anton Dedov
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
//  $Id: parser.cc,v 1.13 2006-09-07 09:35:42 adedov Exp $

#include <stdexcept>
#include <functional>
#include <algorithm>
#include "sysinc.h"
#include "parser.h"
#include "except.h"
#include "parser_specific.h"
#include "value_type.h"
#include "value.h"

using namespace iqxmlrpc;

Parser* Parser::instance_ = 0;


Parser* Parser::instance()
{
  if( !Parser::instance_ )
    Parser::instance_ = new Parser;

  return Parser::instance_;
}


Parser::Parser()
{
  Value_parser* i4p = new Int_parser;
  register_parser( "i4", i4p );
  register_parser( "int", i4p );
  register_parser( "boolean", new Boolean_parser );
  register_parser( "double", new Double_parser );
  register_parser( "string", new String_parser );
  register_parser( "", new String_parser ); // default type
  register_parser( "nil", new Nil_parser );
  register_parser( "base64", new Base64_parser );
  register_parser( "dateTime.iso8601", new Date_time_parser );
  register_parser( "array", new Array_parser );
  register_parser( "struct", new Struct_parser );
}


Parser::~Parser()
{
  clean_types();
  instance_ = 0;
}


void Parser::clean_types()
{
  std::for_each( types.begin(), types.end(),
    std::mem_fun_ref(&Type_desc::clean) );
}


Value* Parser::parse_value( const xmlpp::Node* node )
{
  using namespace xmlpp;

  if( node->get_name() != "value" )
    throw XML_RPC_violation::at_node( node );

  Node *valnode = 0;
  std::string tname;
  get_value_node( node, valnode, tname );

  for( Types_list::const_iterator i = types.begin(); i != types.end(); ++i )
  {
    if( i->xmlrpc_name == tname )
    {
      if (valnode)
        return new Value(i->parser->parse_value(valnode));
      else
        return new Value("");
    }
  }

  throw XML_RPC_violation::caused( "unknown XML-RPC value type '" + tname + "'" );
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
        throw XML_RPC_violation::at_node(*i);
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
    throw XML_RPC_violation::at_node(node);

  return dynamic_cast<xmlpp::Element*>(lst.front());
}


void Parser::get_value_node(
  const xmlpp::Node* node, xmlpp::Node*& valnode, std::string& type )
{
  using namespace xmlpp;

  Node::NodeList childs = node->get_children();
  if( childs.size() == 1 )
  {
    // case: <value>TEXT</value>
    // TEXT should be treated as a value of default type
    TextNode* text = dynamic_cast<TextNode*>(childs.front());
    if( text )
    {
      valnode = const_cast<Node*>(node);
      type = "";
      return;
    }
  }
  else if (childs.size() == 0)
  {
    // case: <value/>
    // should be treated as empty value of default type
    valnode = 0;
    type = "";
    return;
  }

  valnode = single_element(node);
  type = valnode->get_name();
}
