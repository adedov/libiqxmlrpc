//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_response_h_
#define _iqxmlrpc_response_h_

#include <memory>
#include <string>
#include "api_export.h"

namespace iqxmlrpc {

class Response;
class Value;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

//! Build response object from XML-formed string.
LIBIQXMLRPC_API Response parse_response( const std::string& );

//! Dump response to XML.
LIBIQXMLRPC_API std::string dump_response( const Response& );

//! XML-RPC response.
class LIBIQXMLRPC_API Response {
public:
  Response( Value* );
  Response( int fault_code, const std::string& fault_string );

  //! Returns response value or throws iqxmlrpc::Fault in case of fault.
  const Value& value() const;

  //! Check whether response is an XML-RPC Fault Reponse.
  bool is_fault()   const { return !value_; }
  //! Returns fault code of Fault Response.
  int  fault_code() const { return fault_code_; }
  //! Returns fault string of Fault Response.
  const std::string& fault_string() const { return fault_string_; }

private:
  std::shared_ptr<Value> value_;
  int fault_code_;
  std::string fault_string_;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqxmlrpc

#endif
