#include <libiqxmlrpc/response.h>
#include <libiqxmlrpc/parser.h>
#include <libiqxmlrpc/value.h>
#include <libiqxmlrpc/except.h>

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


Response::~Response()
{
  delete value_;
}


xmlpp::Document* Response::to_xml() const
{
  xmlpp::Document* doc = new xmlpp::Document();

  try 
  {
    xmlpp::Element* el = doc->create_root_node( "methodResponse" );
    
    if( is_fault() )
      fault_to_xml( el );
    else
      ok_to_xml( el );
  }
  catch(...)
  {
    delete doc;
    throw;
  }
  
  return doc;
}


const Value& Response::value() const
{
  if( is_fault() )
    throw Fault( fault_code_, fault_string_ );
  
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
    throw Parse_error::at_node(n);
}


inline void Response::parse_param( const xmlpp::Node* node )
{
  Parser *parser = Parser::instance();
  
  xmlpp::Node* param = parser->single_element(node);
  if( param->get_name() != "param" )
    throw Parse_error::at_node(param);
  
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
    throw Parse_error::caused( err );
  
  if( !fault["faultCode"].is_int() || !fault["faultString"].is_string() )
    throw Parse_error::caused( err );
  
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
