#include <iostream>
#include <libiqnet/acceptor.h>
#include <libiqnet/connection.h>
#include <libiqnet/conn_fabric.h>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/request.h>

using namespace iqxmlrpc;
using namespace iqnet;

xmlpp::DomParser xml_parser;
Method_dispatcher dispatcher;
Reactor reactor;


class Get_weather: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};


void Get_weather::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  if( args[0].get_string() != "Krasnoyarsk" )
    throw iqxmlrpc::Fault( 0, "Unknown town." );
  
  iqxmlrpc::Struct s;
  s.insert( "weather", "Snow" );
  s.insert( "temp.C", -8.0 );
  
  retval = s;
}


// ---------------------------------------------------
class Server_connection: public Connection {
  std::string xml_content;
  Response *resp;
  
public:
  Server_connection( int fd, const iqnet::Inet_addr& addr ):
    Connection( fd, addr ), resp(0) {}

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
  
  reactor.register_handler( this, Reactor::OUTPUT );

  xml_parser.parse_memory( xml_content );
  Request req( xml_parser.get_document() );
  Method* m = dispatcher.create_method( req.get_name() );
  
  if( !m )
  {
    resp = new Response( 0, "Unknown XML-RPC method." );
    return;
  }
  
  try {
    Value v(0);
    m->execute( req.get_params(), v );
    resp = new Response( v );
  }
  catch( iqxmlrpc::Fault& f )
  {
    resp = new Response( f.code(), f.what() );
  }
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
    xml_parser.set_substitute_entities(); 
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
