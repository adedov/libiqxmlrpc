//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_request_h_
#define _iqxmlrpc_request_h_

#include <string>
#include <vector>

#include "value.h"

namespace iqxmlrpc {

class Request;
typedef std::vector<Value> Param_list;

//! Build request object from XML-formed string.
LIBIQXMLRPC_API  Request* parse_request( const std::string& );

//! Dump Request to XML.
LIBIQXMLRPC_API std::string dump_request( const Request& );

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

//! Incoming RPC request.
class LIBIQXMLRPC_API Request {
public:
  typedef Param_list::const_iterator const_iterator;

  Request( const std::string& name, const Param_list& params );

  const std::string& get_name()   const { return name; }
  const Param_list&  get_params() const { return params; }

private:
  std::string name;
  Param_list  params;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqxmlrpc

#endif
