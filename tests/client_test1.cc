#include <iostream>
#include <stdexcept>
#include <libiqxmlrpc/http_transport.h>

using namespace iqxmlrpc;
using namespace iqnet;

int main()
{
  try {
    Http_client client( Inet_addr(3344) );
    Param_list pl;
    pl.push_back( Value("Krasnoyarsk") );
    Response resp = client.execute( "get_weather", pl );
    
    if( resp.is_fault() )
    {
      std::cout << "Fault response: " << resp.fault_string() << std::endl;
      return 0;
    }
    
    std::cout << resp.value()["weather"].get_string() << "\n"
              << resp.value()["temp.C"].get_double() << std::endl;
  }
  catch ( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
