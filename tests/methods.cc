#include <iostream>
#include <fstream>
#include <boost/test/test_tools.hpp>
#include "libiqxmlrpc/server.h"
#include "methods.h"

void register_user_methods(iqxmlrpc::Server& s)
{
  s.register_method<Get_weather>( "get_weather" );
  s.register_method<Get_file>( "get_file" );
}

void Get_weather::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  BOOST_MESSAGE("Get_weather method invoked.");

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
  BOOST_MESSAGE("Get_file method invoked.");

  std::ifstream f( args[0].get_string().c_str(), std::ios_base::binary );
  if( !f )
    throw iqxmlrpc::Fault( 0, "File read error." );
    
  std::string s;
  for( int c = f.get(); f && c != EOF; c = f.get() )
    s += c;
    
  retval = iqxmlrpc::Binary_data::from_data( s );
}
