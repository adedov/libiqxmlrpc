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
    Fault( int i, const std::string& s ):
      code_(i), Exception(s) {}
        
    int code() const { return code_; }
  };
};

#endif
