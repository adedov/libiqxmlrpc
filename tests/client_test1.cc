#include <iostream>
#include <stdexcept>
#include <libiqxmlrpc/http_transport.h>

using namespace iqxmlrpc;
using namespace iqnet;

void show_weather( Http_client& client, const std::string& town )
{
  Param_list pl;
  pl.push_back( Value(town) );
  Response resp = client.execute( "get_weather", pl );

  std::cout << "Site: " << town << std::endl;
  
  if( resp.is_fault() )
  {
    std::cout << "Fault response: " << resp.fault_string() << std::endl;
    return;
  }
  
  std::cout 
    << "Weather: " << resp.value()["weather"].get_string() << "\n"
    << "Temp:    " << resp.value()["temp.C"].get_double() 
    << std::endl << std::endl;
}


int main()
{
  try {
    Http_client client( Inet_addr(3344) );
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
