#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>
#include "acceptor.h"
#include "connection.h"
#include "conn_fabric.h"
#include "net_except.h"
#include "inet_addr.h"

using namespace iqnet;


Acceptor::Acceptor( int port, Accepted_conn_fabric* fabric_, Reactor* reactor_ ):
  fabric(fabric_),
  reactor(reactor_)
{
  if( (sock = socket( PF_INET, SOCK_STREAM, 0 )) == -1 )
    throw network_error( "socket" );

  int enable = 1;
  setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable) );
  
  Inet_addr addr( port );
  const sockaddr* saddr = reinterpret_cast<const sockaddr*>(addr.get_sockaddr());
  if( bind( sock, saddr, sizeof(sockaddr_in) ) == -1 )
    throw network_error( "bind" );
  
  listen();
  reactor_->register_handler( this, Reactor::INPUT );
}


Acceptor::~Acceptor()
{
  close( sock );
}


void Acceptor::handle_input( bool& )
{
  accept();
}


inline void Acceptor::listen()
{
  if( ::listen( sock, 5 ) == -1 )
    throw network_error( "listen" );
}


void Acceptor::accept()
{
  struct sockaddr_in addr;
  socklen_t len;
  
  int new_sock = ::accept( sock, reinterpret_cast<sockaddr*>(&addr), &len );  
  if( new_sock == -1 )
    throw network_error( "accept" );

  fabric->create_accepted( new_sock, Inet_addr( addr ) );
  listen();
}


const Inet_addr Acceptor::get_addr_listening() const
{
  struct sockaddr_in addr;
  socklen_t len;
  
  getsockname( sock, reinterpret_cast<sockaddr*>(&addr), &len );
  return Inet_addr( addr );
}
