#include <iostream>
#include "libiqxmlrpc/libiqxmlrpc.h"


class Get_weather: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};


void Get_weather::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  std::cout << "Get_weather method invoked." << std::endl;

//  sleep(10);

  if( args[0].get_string() != "Krasnoyarsk" )
    throw iqxmlrpc::Fault( 0, "Unknown town." );
  
  iqxmlrpc::Struct s;
  s.insert( "weather", "Snow" );
  s.insert( "temp.C", -8.0 );
  
  retval = s;
}
