#include <iostream>
#include <libiqxmlrpc/http.h>

using namespace iqxmlrpc;

int main( int argc, char** argv )
{
  try {
    std::string content = "<?xml version=\"1.0\"?><a/>";
    http::Response response = http::Response::outgoing( content );
    std::cout << response.dump() << std::endl;
  }
  catch( const http::Error_response& e )
  {
    std::cerr << e.dump() << std::endl;
    return 1;
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
