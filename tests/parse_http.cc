#include <iostream>
#include <fstream>
#include <libiqxmlrpc/http.h>

using namespace iqxmlrpc;


std::string read_file( const std::string& filename )
{
  std::ifstream f( filename.c_str() );
  std::string request;
  
  if( !f )
    throw Exception( "No such file: " + filename );
  
  for( char c = f.get(); f && !f.eof(); c = f.get() )
    request += c;
  
  return request;
}


int main( int argc, char **argv )
{
  try {
    std::string filename = argc > 1 ? argv[1] : "data/request.http";
    std::string rstr = read_file( filename );
    
    http::Request req( rstr );
    std::cout 
      << "URI:          '" << req.uri()     << "'" << std::endl
      << "HTTP version: '" << req.version() << "'" << std::endl
      << "User-Agent:   '" << req.agent()   << "'" << std::endl
      << "Host:         '" << req.host()    << "'" << std::endl
      << std::endl
      << " -- content -- " << std::endl
      << req.content()
      << " -- end of content -- " << std::endl;
  }
  catch( const Exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
