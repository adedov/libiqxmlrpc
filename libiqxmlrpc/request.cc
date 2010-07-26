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
//  $Id: request.cc,v 1.8 2006-09-07 09:35:42 adedov Exp $

#include "request.h"

#include "except.h"
#include "parser.h"
#include "value.h"
#include "value_type_xml.h"

#include <libxml++/libxml++.h>

#include <memory>

namespace iqxmlrpc {

Request* parse_request( const std::string& request_string )
{
  try {
    xmlpp::DomParser parser;
    parser.set_substitute_entities();
    parser.parse_memory( request_string );

    return new Request( parser.get_document() );
  }
  catch( xmlpp::exception& e )
  {
    throw Parse_error( e.what() );
  }
}


//-----------------------------------------------------------------------------
Request::Request( const xmlpp::Document* doc )
{
  parse( doc->get_root_node() );
}


Request::Request( const xmlpp::Node* node )
{
  parse( node );
}


Request::Request( const std::string& name_, const Param_list& params_ ):
  name(name_),
  params(params_)
{
}


Request::~Request()
{
}


xmlpp::Document* Request::to_xml() const
{
  using namespace xmlpp;

  std::auto_ptr<Document> doc(new Document);

  Element* el = doc->create_root_node( "methodCall" );
  Element* name_el = el->add_child( "methodName" );
  name_el->add_child_text( name );
  Element* params_el = el->add_child( "params" );

  for( const_iterator i = params.begin(); i != params.end(); ++i )
  {
    Element* p_el = params_el->add_child( "param" );
    value_to_xml(*i, p_el);
  }

  return doc.release();
}


inline void Request::parse( const xmlpp::Node* node )
{
  if( node->get_name() != "methodCall" )
    throw XML_RPC_violation::at_node(node);

  xmlpp::Node::NodeList nlist = Parser::instance()->elements_only( node );
  if( nlist.size() != 2 )
    throw XML_RPC_violation::at_node(node);

  parse_name( nlist.front() );
  parse_params( nlist.back() );
}


inline void Request::parse_name( const xmlpp::Node* node )
{
  using namespace xmlpp;

  if( node->get_name() != "methodName" )
    throw XML_RPC_violation::at_node(node);

  Node::NodeList childs = node->get_children();
  if( childs.size() != 1 )
    throw XML_RPC_violation::at_node(node);

  const TextNode *text = dynamic_cast<const TextNode*>(childs.front());
  if( !text )
    throw XML_RPC_violation::at_node(node);

  name = text->get_content();
}


inline void Request::parse_params( const xmlpp::Node* node )
{
  using namespace xmlpp;
  Parser* parser = Parser::instance();

  if( node->get_name() != "params" )
    throw XML_RPC_violation::at_node(node);

  Node::NodeList childs = parser->elements_only( node );
  for( Node::NodeList::const_iterator i=childs.begin(); i!=childs.end(); ++i )
  {
    if( (*i)->get_name() != "param" )
      throw XML_RPC_violation::at_node(*i);

    Node* param = parser->single_element( *i );
    Value* value = parser->parse_value(param);
    params.push_back( *value );
    delete value;
  }
}

} // namespace iqxmlrpc
