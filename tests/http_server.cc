#include <signal.h>
#include "libiqxmlrpc/server.h"
#include "libiqxmlrpc/http_transport.h"
#include "server_general.h"

using namespace iqnet;
using namespace iqxmlrpc;

Server *server = 0;


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
    Method_dispatcher dispatcher;
    dispatcher.register_method( "get_weather", new Method_factory<Get_weather> );
    server = new Server( 3344, &dispatcher, new Executor_fabric<Serial_executor> );
    server->work<Http_server_connection>();
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
