// This file contains parser classes for specific Value types.
#ifndef _iqxmlrpc_parser_specific_h_
#define _iqxmlrpc_parser_specific_h_

#include <libiqxmlrpc/parser.h>


namespace iqxmlrpc
{
  class Int_parser;
  class String_parser;
  class Boolean_parser;
  class Double_parser;
  class Array_parser;
  class Struct_parser;
};


class iqxmlrpc::Int_parser: public iqxmlrpc::Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};


class iqxmlrpc::String_parser: public iqxmlrpc::Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};


class iqxmlrpc::Boolean_parser: public iqxmlrpc::Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};


class iqxmlrpc::Double_parser: public iqxmlrpc::Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};


class iqxmlrpc::Array_parser: public iqxmlrpc::Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;
};


class iqxmlrpc::Struct_parser: public iqxmlrpc::Value_parser {
public:
  Value_type* parse_value( const xmlpp::Node* ) const;

private:
  std::string get_member_name( const xmlpp::Node* ) const;
};

#endif
