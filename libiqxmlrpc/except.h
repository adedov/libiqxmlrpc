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
//  $Id: except.h,v 1.7 2004-09-19 09:32:38 adedov Exp $

#ifndef _iqxmlrpc_except_h_
#define _iqxmlrpc_except_h_

#include <stdexcept>

namespace xmlpp
{
  class Node;
};

// Exceptions are conformant ot Fault Code Interoperability, version 20010516.
// http://xmlrpc-epi.sourceforge.net/specs/rfc.fault_codes.php
namespace iqxmlrpc 
{
  class Exception;
  class Parse_error;
  class XML_RPC_violation;
  class Unknown_method;
  class Invalid_meth_params;
  class Fault; 
};


//! Base class for iqxmlrpc exceptions.
class iqxmlrpc::Exception: public std::runtime_error {
  int ex_code;
  
public:
  Exception( const std::string& i, int c = -32000 /*undefined error*/ ):
    runtime_error( i ), ex_code(c) {}

  virtual int code() const { return ex_code; }
};


//! XML Parser error.
class iqxmlrpc::Parse_error: public iqxmlrpc::Exception {
public:
  Parse_error( const std::string& d ):
    Exception( "Parser error. " + d, -32700 ) {}
};


//! XML-RPC structures not conforming to spec.
class iqxmlrpc::XML_RPC_violation: public iqxmlrpc::Exception {
public:
  static XML_RPC_violation at_node( const xmlpp::Node* );
  static XML_RPC_violation caused( const std::string&, const xmlpp::Node* = 0 );

  XML_RPC_violation():
    Exception( "Server error. XML-RPC violation.", -32600 ) {}

private:
  XML_RPC_violation( const std::string& s ):
    Exception( "Server error. XML-RPC violation: " + s, -32600 ) {}
};


//! Exception is being thrown when user tries to create 
//! Method object for unregistered name.
class iqxmlrpc::Unknown_method: public iqxmlrpc::Exception {
public:
  Unknown_method( const std::string& name ): 
    Exception( "Server error. Method '" + name + "' not found.", -32601 ) {}
};


//! Invalid method parameters exception.
class iqxmlrpc::Invalid_meth_params: public iqxmlrpc::Exception {
public:
  Invalid_meth_params():
    Exception( "Server error. Invalid method parameters.", -32602 ) {}
};


//! Exception which user should throw from Method to
//! initiate fault response.
class iqxmlrpc::Fault: public iqxmlrpc::Exception {
public:
  class FCI_violation: public std::runtime_error {
  public:
    FCI_violation():
      runtime_error( 
        "You should not specify application specific error codes "
        "in interval [-32768, -32000]." 
      ) {};
  };
  
public:
  Fault( int c, const std::string& s );
};

#endif
