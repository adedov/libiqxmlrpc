#include <iostream>
#include <string>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/client.h"


void show_weather( iqxmlrpc::Client_base& client, const std::string& town )
{
  using namespace iqxmlrpc;
  
  Param_list pl;
  pl.push_back( Value(town) );
  Response resp = client.execute( "get_weather", pl );

  if( resp.is_fault() )
  {
    std::cout << "Fault response: " << resp.fault_string() << std::endl;
    return;
  }
  
  std::cout 
    << "Town:    " << town << std::endl
    << "Weather: " << resp.value()["weather"].get_string() << std::endl
    << "Temp:    " << resp.value()["temp.C"].get_double()  << std::endl
    << std::endl;
}
