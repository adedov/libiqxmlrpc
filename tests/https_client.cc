#include <stdexcept>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "client_general.h"

int main()
{
  try {
    iqnet::ssl::ctx = iqnet::ssl::Ctx::client_only();
    iqxmlrpc::Client<iqxmlrpc::Https_client_connection> client( iqnet::Inet_addr(3344) );

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
