#ifndef _iqxmlrpc_except_h_
#define _iqxmlrpc_except_h_

#include <stdexcept>

namespace xmlpp
{
  class Node;
};


namespace iqxmlrpc 
{
  namespace Fault_code
  {
    enum {
      xml_parser,
      xmlrpc_parser,
      xmlrpc_usage,
      unknown_method,
      first = xml_parser,
      last  = unknown_method
    };
  };

  
  //! Base class for iqxmlrpc exceptions.
  class Exception: public std::runtime_error {
    int code_;
    
  public:
    Exception( const std::string& i, int c = Fault_code::xmlrpc_usage ):
      runtime_error( i ), code_(c) {}
        
    int code() const { return code_; }
  };

  
  //! Common exception class for various error cases 
  //! which can happen during parsing of XML-RPC structures.
  class Parse_error: public Exception {
    enum { code = Fault_code::xmlrpc_parser };
    
  public:
    static Parse_error at_node( const xmlpp::Node* );
    static Parse_error caused( const std::string&, const xmlpp::Node* = 0 );

    Parse_error():
      Exception( "Could not parse XML-RPC data.", code ) {}
        
  private:
    Parse_error( const std::string& d ):
      Exception( "Could not parse XML-RPC data: " + d + ".", code ) {}
  };


  //! Exception which user should throw from Method to
  //! initiate fault response.
  class Fault: public iqxmlrpc::Exception {
  public:
    Fault( int code, const std::string& s ):
      Exception( s, code ) {}
  };
};


#endif
