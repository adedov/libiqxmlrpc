#include <netdb.h>
#include "inet_addr.h"
#include "net_except.h"

using namespace iqnet;


Inet_addr::Inet_addr( const std::string& host_, int port_ ):
  host(host_), port(port_)
{
  struct hostent *hent = gethostbyname( host.c_str() );
  if( !hent )
    throw network_error( "gethostbyname" );
  
  sa.sin_family = PF_INET;
  sa.sin_port = htons(port);
  memcpy( (char*)&sa.sin_addr, (char*)hent->h_addr, hent->h_length );
}


Inet_addr::Inet_addr( int port_ ):
  host("localhost"), port(port_)
{
  sa.sin_family = PF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr.s_addr = INADDR_ANY;
}


Inet_addr::Inet_addr( const struct sockaddr_in& sa_ ):
  sa(sa_)
{
  host = inet_ntoa( sa.sin_addr );
  port = ntohs( sa.sin_port );
}
