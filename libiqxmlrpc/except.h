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
//  $Id: except.h,v 1.6 2004-04-22 07:43:19 adedov Exp $

#ifndef _iqxmlrpc_except_h_
#define _iqxmlrpc_except_h_

#include <stdexcept>

namespace xmlpp
{
  class Node;
};


namespace iqxmlrpc 
{
  //! Base class for iqxmlrpc exceptions.
  class Exception: public std::runtime_error {
  public:
    Exception( const std::string& i ):
      runtime_error( i ) {}

    virtual int code() const { return -1; }
  };

  
  //! Common exception class for various error cases 
  //! which can happen during parsing of XML-RPC structures.
  class Parse_error: public Exception {
  public:
    static Parse_error at_node( const xmlpp::Node* );
    static Parse_error caused( const std::string&, const xmlpp::Node* = 0 );

    Parse_error():
      Exception( "Could not parse XML-RPC data." ) {}
        
  private:
    Parse_error( const std::string& d ):
      Exception( "Could not parse XML-RPC data: " + d + "." ) {}
  };


  //! Exception which user should throw from Method to
  //! initiate fault response.
  class Fault: public iqxmlrpc::Exception {
    int code_;
    
  public:
    Fault( int c, const std::string& s ):
      Exception( s ), code_(c) {}
        
    int code() const { return code_; }
  };
};

#endif
