#include <unistd.h>
#include <iostream>
#include <libiqxmlrpc/http_transport.h>

using namespace iqxmlrpc;
using namespace iqnet;


Http_reaction_connection::Http_reaction_connection( int fd, const Inet_addr& addr ):
  Connection( fd, addr ),
  reactor(0),
  response(0),
  server(0)
{
}
    

Http_reaction_connection::~Http_reaction_connection()
{
  delete server;
  delete response;
}


void Http_reaction_connection::post_accept()
{
  set_non_blocking(true);
  reactor->register_handler( this, Reactor::INPUT );
}


void Http_reaction_connection::finish()
{
  delete this;
}


void Http_reaction_connection::handle_input( bool& terminate )
{   
  try {
    char buf[256];
    int n = recv( buf, sizeof(buf) );
    
    if( !n )
    {
      terminate = true;
      return;
    }
    
    if( !server->read_request( std::string(buf, n) ) )
      return;
    
    response = server->execute();
  }
  catch( const http::Error_response& e )
  {
    delete response;
    response = new http::Packet(e);
  }

  reactor->unregister_handler( this, Reactor::INPUT );
  reactor->register_handler( this, Reactor::OUTPUT );
}


void Http_reaction_connection::handle_output( bool& terminate )
{
  send_str( response->dump() );
  delete response;
  response = 0;
  terminate = true;
}


// --------------------------------------------------------------------------
Http_server::Http_server( int port, Method_dispatcher* disp ):
  reactor(),
  acceptor(0),
  cfabric( new C_fabric( disp, &reactor ) ),
  exit_flag(false) 
{
  acceptor = new Acceptor( port, cfabric, &reactor );
}


Http_server::~Http_server()
{
  delete acceptor;
  delete cfabric;
}


void Http_server::work()
{
  while( !exit_flag )
    reactor.handle_events();
}


// --------------------------------------------------------------------------
Http_client::Http_client( const iqnet::Inet_addr& a, const std::string& uri ):
  http::Client( uri ),
  addr(a),
  conn(0),
  ctr(a)
{
  set_client_host( iqnet::get_host_name() );
}


Http_client::~Http_client()
{
  delete conn;
}


void Http_client::send_request( const http::Packet& packet )
{
  conn = ctr.connect();
  std::string req( packet.dump() );
  conn->send( req.c_str(), req.length() );
}


void Http_client::recv_response()
{
  for( bool r = false; !r; )
  {
    char buf[256];
    bzero( buf, 256 );
    unsigned sz = conn->recv( buf, 255 );
    r = read_response( sz ? std::string(buf, sz) : std::string("") );
  }
}
