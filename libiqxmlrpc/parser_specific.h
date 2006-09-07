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
//  $Id: parser_specific.h,v 1.8 2006-09-07 04:45:21 adedov Exp $

// This file contains parser classes for specific Value types.
#ifndef _iqxmlrpc_parser_specific_h_
#define _iqxmlrpc_parser_specific_h_

#include "parser.h"

namespace iqxmlrpc {

class Nil_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};

class Int_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};

class String_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};

class Boolean_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};

class Double_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};

class Base64_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};

class Date_time_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};

class Array_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};

class Struct_parser: public Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;

private:
  std::string get_member_name( const xmlpp::Node* ) const;
};

} // namespace iqxmlrpc

#endif
