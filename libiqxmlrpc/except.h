//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_except_h_
#define _iqxmlrpc_except_h_

#include "api_export.h"

#include <stdexcept>

// Exceptions are conformant ot Fault Code Interoperability, version 20010516.
// http://xmlrpc-epi.sourceforge.net/specs/rfc.fault_codes.php
namespace iqxmlrpc
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4275)
#endif

//! Base class for iqxmlrpc exceptions.
class LIBIQXMLRPC_API Exception: public std::runtime_error {
  int ex_code;

public:
  Exception( const std::string& i, int c = -32000 /*undefined error*/ ):
    runtime_error( i ), ex_code(c) {}

  virtual int code() const { return ex_code; }
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

//! XML Parser error.
class LIBIQXMLRPC_API Parse_error: public Exception {
public:
  Parse_error( const std::string& d ):
    Exception(std::string("Parser error. ") += d, -32700) {}
};

//! XML Parser error.
class LIBIQXMLRPC_API XmlBuild_error: public Exception {
public:
  XmlBuild_error( const std::string& d ):
    Exception(std::string("XML build error. ") += d, -32705) {}
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4275)
#endif

//! XML-RPC structures not conforming to spec.
class LIBIQXMLRPC_API XML_RPC_violation: public Exception {
public:
  XML_RPC_violation():
    Exception("Server error. XML-RPC violation.", -32600) {}

  XML_RPC_violation( const std::string& s ):
    Exception(std::string("Server error. XML-RPC violation: ") += s, -32600) {}
};

//! Exception is being thrown when user tries to create
//! Method object for unregistered name.
class LIBIQXMLRPC_API Unknown_method: public Exception {
public:
  Unknown_method( const std::string& name ):
    Exception((std::string("Server error. Method '") += name) += "' not found.", -32601) {}
};

//! Invalid method parameters exception.
class LIBIQXMLRPC_API Invalid_meth_params: public Exception {
public:
  Invalid_meth_params():
    Exception( "Server error. Invalid method parameters.", -32602 ) {}
};

//! Exception which user should throw from Method to
//! initiate fault response.
class LIBIQXMLRPC_API Fault: public Exception {
public:
  Fault( int c, const std::string& s ):
    Exception(s, c) {}
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqxmlrpc

#endif
