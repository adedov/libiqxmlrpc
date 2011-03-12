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
//  $Id: parser_specific.cc,v 1.12 2006-09-25 08:19:23 adedov Exp $

#include "parser_specific.h"

#include "except.h"
#include "utf_conv.h"
#include "value.h"
#include "value_type.h"

#include <sstream>

using namespace xmlpp;
using namespace iqxmlrpc;

namespace {

const Element*
require_element( const Node* node )
{
  const Element *el = dynamic_cast<const Element*>(node);

  if( !el )
    throw XML_RPC_violation::at_node(node);

  return el;
}

std::string
get_node_text( const Node* node, bool allow_empty = false )
{
  const Element* el = require_element(node);

  if (!el->has_child_text()) {
    if (allow_empty)
      return std::string();

    throw XML_RPC_violation::at_node(node);
  }

  return el->get_child_text()->get_content();
}

} // anonymous namespace

Value_type* Nil_parser::parse_value( const Node* node ) const
{
  // TODO: check for actual nil tag
  return new Nil();
}


Value_type* Int_parser::parse_value( const Node* node ) const
{
  std::stringstream ss( get_node_text(node) );
  int i = 0;
  ss >> i;

  if( !ss || !ss.eof() )
    throw XML_RPC_violation::caused( "bad int representation", node );

  Value_type *v = new Int( i );
  return v;
}


Value_type* String_parser::parse_value( const Node* node ) const
{
  return new String( get_node_text(node, true) );
}


Value_type* Boolean_parser::parse_value( const Node* node ) const
{
  std::string s( get_node_text(node) );
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


Value_type* Double_parser::parse_value( const Node* node ) const
{
  std::stringstream ss( get_node_text(node) );
  double d = 0;
  ss >> d;

  if( !ss || !ss.eof() )
    throw XML_RPC_violation::caused( "bad double representation", node );

  return new Double( d );
}


Value_type* Base64_parser::parse_value( const Node* node ) const
{
  return Binary_data::from_base64( get_node_text(node, true) );
}


Value_type* Date_time_parser::parse_value( const Node* node ) const
{
  return new Date_time( get_node_text(node) );
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
    Value_ptr v( Parser::instance()->parse_value(*i) );
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
    Value_ptr value( Parser::instance()->parse_value(mbr.back()) );
    s.insert( name, value );
  }

  return s.clone();
}


inline std::string Struct_parser::get_member_name( const Node* node ) const
{
  std::string name = get_node_text(node);

  if( name.find_first_not_of( "\t\n\r " ) == std::string::npos )
    throw XML_RPC_violation::at_node(node);

  return name;
}
