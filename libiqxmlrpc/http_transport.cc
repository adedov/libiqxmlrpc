#include <iostream>
#include <libiqxmlrpc/http_transport.h>

using namespace iqxmlrpc;
using namespace iqnet;


Http_reaction_connection::Http_reaction_connection
(
  int sock,
  const Inet_addr& addr,
  Reactor* rctr,
  Method_dispatcher* disp
):
  Connection( sock, addr ),
  Server( disp ),
  reactor( rctr ),
  response(0)
{
}
    

Http_reaction_connection::~Http_reaction_connection()
{
  delete response;
}


void Http_reaction_connection::post_init()
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
    
    if( !read_request( std::string(buf, n) ) )
      return;
    
    response = execute();
  }
  catch( const http::Error_response& e )
  {
    delete response;
    response = new http::Packet(e);
  }

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
