#include <iostream>
#include <libiqxmlrpc/except.h>
#include <libiqxmlrpc/method.h>
#include <libiqxmlrpc/request.h>
#include <libiqxmlrpc/response.h>


class Get_weather: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};


void Get_weather::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  if( args[0].get_string() != "Krasnoyarsk" )
    throw iqxmlrpc::Fault( 0, "Unknown town." );
  
  iqxmlrpc::Struct s;
  s.insert( "weather", "Snow" );
  s.insert( "temp.C", -8.0 );
  
  retval = s;
}


int main( int argc, char** argv )
{
  using namespace iqxmlrpc;
  
  try {
    Method_dispatcher disp;
    disp.register_method( "get_weather", new Method_factory<Get_weather> );
    
    xmlpp::DomParser parser;
    parser.set_substitute_entities(); 
    parser.parse_file( argc > 1 ? argv[1] : "data/request.xml" );
    Request req( parser.get_document() );
    
    Method* m = disp.create_method( req.get_name() );
    if( !m )
    {
      std::cout << "No such method." << std::endl;
      return 1;
    }
    
    Value v(0);
    m->execute( req.get_params(), v );
    Response resp( v );
    std::cout << resp.to_xml()->write_to_string_formatted() << std::endl;
  }
  catch( iqxmlrpc::Fault& f )
  {
    Response resp( f.code(), f.what() );
    std::cout << resp.to_xml()->write_to_string_formatted() << std::endl;
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
