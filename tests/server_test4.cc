#include <signal.h>
#include <iostream>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/http_transport.h>

using namespace iqxmlrpc;
using namespace iqnet;

Method_dispatcher dispatcher;
Http_server *server = 0;

const int user_fault = Fault_code::last+1;


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


void sigint_handler( int )
{
  std::cout << "Doing graceful exit..." << std::endl;
  
  if( server )
    server->set_exit_flag();
}


int main()
{
  signal( SIGINT, sigint_handler );

  try {
    dispatcher.register_method( "get_weather", new Method_factory<Get_weather> );
    server = new Http_server( 3344, &dispatcher );
    server->work();
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
