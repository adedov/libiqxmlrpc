#include <iostream>
#include <stdexcept>
#include "libiqxmlrpc/response.h"
#include "libiqxmlrpc/value.h"

using namespace iqxmlrpc;


void test_ok_response( const std::string& filename )
{
  xmlpp::DomParser parser;
  parser.set_substitute_entities(); 
  parser.parse_file( filename );

  Response resp( parser.get_document() );
  std::cout 
    << "Weather:     " << resp.value()["weather"].get_string() << std::endl
    << "Temperature: " << resp.value()["temp.C"].get_double() << std::endl
    << std::endl;
  
  // Build document
  xmlpp::Document* doc = resp.to_xml();
  std::cout << doc->write_to_string_formatted() << std::endl;
}


void test_fault_response( const std::string& filename )
{
  xmlpp::DomParser parser;
  parser.set_substitute_entities(); 
  parser.parse_file( "data/response_fault.xml" );

  Response resp( parser.get_document() );
  if( !resp.is_fault() )
    throw std::runtime_error( "Not a fault response." );

  std::cout 
    << "Fault code:   " << resp.fault_code() << std::endl
    << "Fault string: " << resp.fault_string() << std::endl
    << std::endl;
  
  // Build document
  xmlpp::Document* doc = resp.to_xml();
  std::cout << doc->write_to_string_formatted() << std::endl;
}


int main( int argc, char** argv )
{
  try
  {
    test_ok_response( argc > 1 ? argv[1] : "data/response.xml" );
    test_fault_response( argc > 2 ? argv[2] : "data/response_fault.xml" );
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
