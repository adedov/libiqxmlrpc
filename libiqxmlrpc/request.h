#ifndef _iqxmlrpc_request_h_
#define _iqxmlrpc_request_h_

#include <string>
#include <vector>
#include <libxml++/libxml++.h>
#include <libiqxmlrpc/value.h>


namespace iqxmlrpc 
{
  class Value;
  class Request;
    
  typedef std::vector<Value> Param_list;  
};


//! Incoming RPC request.
class iqxmlrpc::Request {
public:
  typedef Param_list::const_iterator const_iterator;

private:
  std::string name;
  Param_list  params;

public:
  Request( const xmlpp::Document* );
  Request( const xmlpp::Node* );
  Request( const std::string& name, const Param_list& params );
  virtual ~Request();

  xmlpp::Document* to_xml() const;

  const std::string& get_name()   const { return name; }
  const Param_list&  get_params() const { return params; }
  
private:
  void parse( const xmlpp::Node* );
  void parse_name( const xmlpp::Node* );
  void parse_params( const xmlpp::Node* );
};


#endif
