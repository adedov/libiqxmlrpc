#include <iostream>
#include "libiqxmlrpc/value.h"
#include "libiqxmlrpc/parser.h"

using namespace iqxmlrpc;

int main( int argc, char **argv )
{
  try
  {
    xmlpp::DomParser parser;
    parser.set_substitute_entities(); 
    parser.parse_file( argc > 1 ? argv[1] : "data/value.xml" );

    if( !parser )
    {
      return 1;
    }
    
    xmlpp::Node* node = parser.get_document()->get_root_node();
    if( !node )
      return 1;
    
    Value* value = Parser::instance()->parse_value(node);
    Value& v = *value;

    std::cout << "Books database:" << std::endl;
    for( int i=0; i < v["books"].size(); i++ )
    {
      Value& book = v["books"][i];
      std::cout 
        << "  Author: " << book["author"].get_string() << std::endl
        << "  Title:  " << book["title"].get_string() << std::endl
        << "  pages:  " << book["pages"].get_int() << std::endl
        << "  price:  " << book["price"].get_double() << std::endl
        << std::endl;
    }

    std::cout << "Value type tests:" << std::endl
      << "  int:    " << v["type.tests"][0].get_int() << std::endl
      << "  double: " << v["type.tests"][1].get_double() << std::endl
      << "  bool:   " << v["type.tests"][2].get_bool() << std::endl
      << "  string: " << v["type.tests"][3].get_string() << std::endl
      << std::endl;

    // Build document
    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node( "value" );
    v.to_xml( root, true );
    std::cout << doc.write_to_string_formatted() << std::endl;
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
                                                                                                                            
  return 0;
}
