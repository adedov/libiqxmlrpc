#include <iostream>
#include "libiqnet/connection.h"
#include "libiqnet/connector.h"

using namespace iqnet;

class Client: public Connection {
public:
  Client( int s, const iqnet::Inet_addr& a ):
    Connection( s, a ) {}
      
  void post_connect()
  {
    std::cout << "Connected." << std::endl;
  }
  
  void finish() {}
};


int main()
{
  try {
    Connector<Client> ctr( Inet_addr( "localhost", 3344 ) );
    Client* c = ctr.connect();

    std::string s = c->recv_str();
    std::cout << "Received: " << s << std::endl;
    c->send_str( s + "\n\r" );
    std::cout << c->recv_str() << std::endl;
  }
  catch( const std::exception& e )
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
