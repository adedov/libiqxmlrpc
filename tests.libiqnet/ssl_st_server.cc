#include <iostream>
#include <libiqnet/acceptor.h>
#include <libiqnet/ssl_connection.h>
#include <libiqnet/conn_fabric.h>

using namespace iqnet;

Reactor reactor;

class My_connection: public ssl::Reaction_connection {
  char* buf;
  
public:
  My_connection( int fd, const iqnet::Inet_addr& addr ):
    Reaction_connection( fd, addr, ssl::ctx, &::reactor ) 
  {
    buf = new char[256];
    bzero( buf, 256 );
  }

  ~My_connection()
  {
    delete[] buf;
  }
    
  void finish()
  {
    std::cout << "Client closed." << std::endl;
    delete this;
  }
  
  void accept_succeed()
  {
    std::cout << "New connection established." << std::endl;
    reg_send( "Hello!\n", 7 );
  }
  
  void recv_succeed( bool& terminate, int req_len, int real_len )
  {
    std::string s = real_len ? std::string( buf, real_len ) : "";
    std::cout << "recv succeed: " << s << std::endl;
    terminate = true;
  }
  
  void send_succeed( bool& terminate )
  {
    std::cout << "send succeed" << std::endl;
    reg_recv( buf, 256 );
  }
};


int main()
{
  try {
    ssl::ctx = ssl::Ctx::server_ctx( "data/cert.pem", "data/pk.pem" );
  
    Acceptor acceptor( 3344, new Serial_conn_fabric<My_connection>, &reactor );
    std::cout << "Accepting connections on port 3344..." << std::endl;
    
    while( true )
    {
      reactor.handle_events();
    }
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
