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
//  $Id: response.cc,v 1.7 2004-10-19 09:00:05 adedov Exp $

#include <memory>
#include "response.h"
#include "parser.h"
#include "value.h"
#include "except.h"

using namespace iqxmlrpc;


Response::Response( const xmlpp::Document* doc ):
  value_(0)
{
  parse( doc->get_root_node() );
}


Response::Response( const xmlpp::Node* node ):
  value_(0)
{
  parse( node );
}


Response::Response( const Value& v ):
  value_(new Value(v))
{
}


Response::Response( int fcode, const std::string& fstring ):
  value_(0),
  fault_code_(fcode),
  fault_string_(fstring)
{
}


inline void Response::assign( const Response& r )
{
  value_ = r.value_;
  fault_code_ = r.fault_code_;
  fault_string_ = r.fault_string_;
}


Response::Response( const Response& r )
{
  assign( r );
}


Response& Response::operator =( const Response& r )
{
  delete value_;
  assign( r );
  return *this;
}


Response::~Response()
{
  delete value_;
}


xmlpp::Document* Response::to_xml() const
{
  std::auto_ptr<xmlpp::Document> doc( new xmlpp::Document() );
  xmlpp::Element* el = doc->create_root_node( "methodResponse" );
  
  if( is_fault() )
    fault_to_xml( el );
  else
    ok_to_xml( el );
  
  return doc.release();
}


const Value& Response::value() const
{
  if( is_fault() )
    throw iqxmlrpc::Exception( fault_string_, fault_code_ );
  
  return *value_;
}


void Response::parse( const xmlpp::Node* node )
{
  xmlpp::Node* n = Parser::instance()->single_element( node );
  
  if( n->get_name() == "params" )
    parse_param( n );
  else if( n->get_name() == "fault" )
    parse_fault( n );
  else 
    throw XML_RPC_violation::at_node(n);
}


inline void Response::parse_param( const xmlpp::Node* node )
{
  Parser *parser = Parser::instance();
  
  xmlpp::Node* param = parser->single_element(node);
  if( param->get_name() != "param" )
    throw XML_RPC_violation::at_node(param);
  
  xmlpp::Node* valnode = parser->single_element(param);
  value_ = parser->parse_value( valnode );
}


inline void Response::parse_fault( const xmlpp::Node* node )
{
  xmlpp::Node* valnode = Parser::instance()->single_element(node);
  Value* fault_ = Parser::instance()->parse_value(valnode);
  Value& fault = *fault_;
  
  std::string err( "malformed structure of fault response." );
  
  if( !fault.has_field("faultCode") || !fault.has_field("faultString") )
    throw XML_RPC_violation::caused( err );
  
  if( !fault["faultCode"].is_int() || !fault["faultString"].is_string() )
    throw XML_RPC_violation::caused( err );
  
  fault_code_   = fault["faultCode"];
  fault_string_ = fault["faultString"].get_string();
  
  delete fault_;
}


inline void Response::ok_to_xml( xmlpp::Node* p ) const
{
  using namespace xmlpp;
  
  Element* params_el = p->add_child( "params" );
  Element* param_el  = params_el->add_child( "param" );
  value_->to_xml( param_el );
}


inline void Response::fault_to_xml( xmlpp::Node* p ) const
{
  using namespace xmlpp;
  
  Element* fault_el = p->add_child( "fault" );
  
  Struct fault;
  fault.insert( "faultCode", fault_code_ );
  fault.insert( "faultString", fault_string_ );
  Value v( fault );
  v.to_xml( fault_el );
}
