#include <iostream>
#include <fstream>
#include "libiqxmlrpc/libiqxmlrpc.h"


class Get_weather: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};


class Get_file: public iqxmlrpc::Method {
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


void Get_file::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  std::ifstream f( args[0].get_string().c_str(), std::ios_base::binary );
  if( !f )
    throw iqxmlrpc::Fault( 0, "File read error." );
    
  std::string s;
  for( int c = f.get(); f && c != EOF; c = f.get() )
    s += c;
    
  retval = iqxmlrpc::Binary_data::from_data( s );
}
