#include <iostream>
#include <libiqnet/acceptor.h>
#include <libiqnet/connection.h>
#include <libiqnet/conn_fabric.h>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/request.h>
#include <libiqxmlrpc/null_transport.h>

using namespace iqxmlrpc;
using namespace iqnet;

Method_dispatcher dispatcher;
Reactor reactor;

const int user_fault = Fault_code::last+1;


class Get_weather: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};


void Get_weather::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  if( args[0].get_string() != "Krasnoyarsk" )
    throw iqxmlrpc::Fault( user_fault, "Unknown town." );
  
  iqxmlrpc::Struct s;
  s.insert( "weather", "Snow" );
  s.insert( "temp.C", -8.0 );
  
  retval = s;
}


// ---------------------------------------------------
class Server_connection: public Connection, protected Server {
  std::string xml_content;
  Response *resp;
  
public:
  Server_connection( int fd, const iqnet::Inet_addr& addr ):
    Connection( fd, addr ), Server(&dispatcher), resp(0) {}

  void post_init()
  {
    set_non_blocking(true);
    reactor.register_handler( this, Reactor::INPUT );
  }
  
  void finish() { delete this; }
  
  void handle_input( bool& );
  void handle_output( bool& );
};


void Server_connection::handle_input( bool& )
{
  char buf[256];
  int n = recv( buf, sizeof(buf) );
  xml_content += std::string( buf, n );
  
  if( n == sizeof(buf) )
    return;
  
  resp = new Response( execute(xml_content) );
  reactor.register_handler( this, Reactor::OUTPUT );
}


void Server_connection::handle_output( bool& terminate )
{
  std::string resp_str = resp->to_xml()->write_to_string_formatted();
  delete resp;
  
  send_str( resp_str );
  terminate = true;
}


int main()
{
  try {
    dispatcher.register_method( "get_weather", new Method_factory<Get_weather> );

    Acceptor acceptor( 3344, new Serial_conn_fabric<Server_connection>, &reactor );
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
