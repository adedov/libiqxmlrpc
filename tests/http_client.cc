#include <stdexcept>
#include <libiqxmlrpc/http_transport.h>
#include "client_general.h"

int main()
{
  try {
    iqxmlrpc::Http_client client( iqnet::Inet_addr(3344) );
    
    show_weather( client, "Krasnoyarsk" );
    show_weather( client, "New York" );
  }
  catch ( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
