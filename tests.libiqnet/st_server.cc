#include <iostream>
#include "libiqnet/acceptor.h"
#include "libiqnet/connection.h"
#include "libiqnet/conn_fabric.h"

using namespace iqnet;

Reactor reactor;

class My_connection: public Connection {
  std::string data;
  
public:
  My_connection( int fd, const iqnet::Inet_addr& addr ):
    Connection( fd, addr ) {}

  void post_accept()
  {
    std::cout << "New connection established." << std::endl;
    reactor.register_handler( this, Reactor::OUTPUT );
  }
  
  void finish()
  {
    std::cout << "Client closed." << std::endl;
    delete this;
  }
  
  void handle_input( bool& )
  {
    data = recv_str();
    std::cout << data << std::endl;
    reactor.unregister_handler( this, Reactor::INPUT );
    reactor.register_handler( this, Reactor::OUTPUT );
  }
  
  void handle_output( bool& terminate )
  {
    if( data.empty() )
    {
      send_str( "Welcome! Please type me anything...\n" );
      reactor.unregister_handler( this, Reactor::OUTPUT );
      reactor.register_handler( this, Reactor::INPUT );
    }
    else
    {
      send_str( "Thanks, bye\n" );
      terminate = true;
    }
  }
};


int main()
{
  try {
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
