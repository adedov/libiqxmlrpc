#include <iostream>
#include <libiqxmlrpc/libiqxmlrpc.h>


const int user_fault = iqxmlrpc::Fault_code::last+1;


class Get_weather: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};


void Get_weather::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  if( args[0].get_string() != "Krasnoyarsk" )
    throw iqxmlrpc::Fault( user_fault, "Unknown town." );
  
  iqxmlrpc::Struct s;
  s.insert( "weather", "Snow" );
  s.insert( "temp.C", -8.0 );
  
  retval = s;
}
