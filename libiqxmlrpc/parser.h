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
//  $Id: parser.h,v 1.5 2006-09-04 12:13:31 adedov Exp $

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
    std::string   xmlrpc_name;
    Value_parser *parser;

    void clean() { delete parser; }

    Type_desc( const std::string& xn, Value_parser* pr ):
      xmlrpc_name(xn), parser(pr) {}
  };

  typedef std::vector< Type_desc > Types_list;
  Types_list types;

  static Parser* instance_;

public:
  static Parser* instance();

  ~Parser();

  Value* parse_value( const xmlpp::Node* );

  //! Registers Value_parser for specified type.
  void register_parser( const std::string& xmlrpc_type_name, Value_parser* p )
  {
    types.push_back( Type_desc(xmlrpc_type_name, p) );
  }

  //! \name Various parsers interface
  //! \{
  xmlpp::Node::NodeList elements_only( const xmlpp::Node* );
  xmlpp::Element* single_element( const xmlpp::Node* );
  //! \}

private:
  Parser();

  void clean_types();
  void get_value_node( const xmlpp::Node*, xmlpp::Node*&, std::string& );
};

#endif
