#include <iostream>
#include <libiqxmlrpc/request.h>
#include <libiqxmlrpc/value.h>

using namespace iqxmlrpc;

int main( int argc, char** argv )
{
  try
  {
    xmlpp::DomParser parser;
    parser.set_substitute_entities(); 
    parser.parse_file( argc > 1 ? argv[1] : "data/request.xml" );

    if( !parser )
    {
      return 1;
    }
    
    Request req( parser.get_document() );
    std::cout 
      << "methodName: " << req.get_name() << std::endl
      << "Params (" << req.get_params().size() << "):" << std::endl
      << "  0: " << req.get_params()[0].get_string() << std::endl
      << "  1: " << req.get_params()[1].get_string() << std::endl
      << std::endl << std::endl;
    
    // Build document
    xmlpp::Document* doc = req.to_xml();
    std::cout << doc->write_to_string_formatted() << std::endl;
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
                                                                                                                            
  return 0;
}
