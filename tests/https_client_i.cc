// Intensive testing HTTPS client

#include <stdexcept>
#include <libiqxmlrpc/https_transport.h>
#include "client_general.h"

int main()
{
  try {
    iqnet::ssl::ctx = iqnet::ssl::Ctx::client_only();
    iqxmlrpc::Https_client client( iqnet::Inet_addr("localhost", 3344) );

    while( true )
    {
      show_weather( client, "Krasnoyarsk" );
      show_weather( client, "New York" );
    }
  }
  catch ( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
