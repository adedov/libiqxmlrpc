#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "connector.h"
#include "net_except.h"

using namespace iqnet;


int Connector_base::socket_connect()
{
  int sock = socket( PF_INET, SOCK_STREAM, 0 );
  if( sock == -1 )
    throw network_error( "socket" );

  int enable = 1;
  setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable) );

  const sockaddr* saddr = 
    reinterpret_cast<const sockaddr*>(peer_addr.get_sockaddr());
  
  if( ::connect(sock, saddr, sizeof(sockaddr_in)) )
    throw network_error( "connect" );
  
  return sock;
}
