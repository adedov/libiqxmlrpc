// HTTPS server test

#include <signal.h>
#include <iostream>
#include <libiqnet/ssl_lib.h>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/https_transport.h>

using namespace iqxmlrpc;
using namespace iqnet;

Https_server* server = 0;
Method_dispatcher dispatcher;

const int user_fault = Fault_code::last+1;


class Get_weather: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};


void Get_weather::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  std::cout << "get_weather execute" << std::endl;
  
  if( args[0].get_string() != "Krasnoyarsk" )
    throw iqxmlrpc::Fault( user_fault, "Unknown town." );
  
  iqxmlrpc::Struct s;
  s.insert( "weather", "Snow" );
  s.insert( "temp.C", -8.0 );
  
  retval = s;
}


void sigint_handler( int )
{
  std::cout << "Doing graceful exit..." << std::endl;
  
  if( server )
    server->set_exit_flag();
}


int main()
{
  ssl::ctx = ssl::Ctx::server_only( "data/cert.pem", "data/pk.pem" );
  signal( SIGINT, sigint_handler );

  try {
    dispatcher.register_method( "get_weather", new Method_factory<Get_weather> );
    server = new Https_server( 3344, &dispatcher );
    server->work();
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
