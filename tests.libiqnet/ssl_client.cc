#include <iostream>
#include <string.h>
#include "libiqnet/ssl_connection.h"
#include "libiqnet/connector.h"
#include "libiqnet/net_except.h"

using namespace iqnet;

class SSL_Client: public ssl::Connection {
public:
  SSL_Client( int s, const iqnet::Inet_addr& a ):
    ssl::Connection( s, a ) {}
      
  void post_connect()
  {
    ssl::Connection::post_connect();
    std::cout << "Connected." << std::endl;
  }
  
  void finish() {}
};


int main()
{
  try {
    ssl::ctx = ssl::Ctx::client_only();

    Connector<SSL_Client> ctr( Inet_addr( "localhost", 3344 ) );
    SSL_Client* c = ctr.connect();

    std::string s = c->recv_str();
    std::cout << "Received: " << s << std::endl;
    c->send_str( s + "\n" );
    std::cout << c->recv_str() << std::endl;
  }
  catch( const std::exception& e )
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
