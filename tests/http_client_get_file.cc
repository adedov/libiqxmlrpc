#include <iostream>
#include <fstream>
#include <stdexcept>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/http_client.h"


void get_file( iqxmlrpc::Client_base& client, const std::string& file )
{
  using namespace iqxmlrpc;
  
  Param_list pl;
  pl.push_back( Value(file) );
  Response resp = client.execute( "get_file", pl );

  if( resp.is_fault() )
  {
    std::cout << "Fault response: " << resp.fault_string() << std::endl;
    return;
  }
  
  std::ofstream of( std::string(file+".received").c_str(), std::ios_base::binary );
  of << resp.value().get_binary().get_data();
}


int main()
{
  try {
    iqxmlrpc::Client<iqxmlrpc::Http_client_connection> client( iqnet::Inet_addr(3344) );
//    client.set_timeout( 3 );

    get_file( client, "bigfile" );    
  }
  catch ( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
