#ifndef _iqxmlrpc_response_h_
#define _iqxmlrpc_response_h_

#include <string>
#include <libxml++/libxml++.h>


namespace iqxmlrpc 
{
  class Value;
  class Response;
};


//! XML-RPC response.
class iqxmlrpc::Response {
private:
  Value* value_;
  int fault_code_;
  std::string fault_string_;
  
public:
  Response( const xmlpp::Document* );
  Response( const xmlpp::Node* );
  Response( const Value& );
  Response( int fault_code, const std::string& fault_string );
  virtual ~Response();

  xmlpp::Document* to_xml() const;

  //! Returns response value or throws Fault in case of fault.
  const Value& value() const;

  bool is_fault()   const { return !value_; }
  int  fault_code() const { return fault_code_; }
  const std::string& fault_string() const { return fault_string_; }
  
private:
  void parse( const xmlpp::Node* );
  void parse_param( const xmlpp::Node* );
  void parse_fault( const xmlpp::Node* );

  void ok_to_xml( xmlpp::Node* ) const;
  void fault_to_xml( xmlpp::Node* ) const;
};

#endif
