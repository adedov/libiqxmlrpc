#include <libxml++/libxml++.h>
#include <sstream>
#include <libiqxmlrpc/except.h>

using namespace iqxmlrpc;


Parse_error Parse_error::at_node( const xmlpp::Node* node )
{
  std::stringstream s;
  s << "format violation at line " << node->get_line();
  return Parse_error( s.str() );
}


Parse_error Parse_error::caused( const std::string& s, const xmlpp::Node* node )
{
  std::string errstr(s);
  
  if( node )
  {
    std::stringstream s;
    s << " at line " << node->get_line();
    errstr += s.str();
  }
  
  return Parse_error( errstr );
}
