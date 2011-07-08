//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "inet_addr.h"

#include "net_except.h"

using namespace iqnet;


std::string iqnet::get_host_name()
{
  char buf[1024];
  buf[1023] = 0;
  ::gethostname( buf, sizeof(buf) );

  return buf;
}


Inet_addr::Inet_addr( const std::string& host_, int port_ ):
  host(host_), port(port_)
{
  struct hostent* hent = 0;

#ifndef _WINDOWS
  struct hostent hent_local;
  char buf[1024];
  int local_h_errno = 0;
  ::gethostbyname_r( host.c_str(), &hent_local, buf, sizeof(buf), &hent, &local_h_errno );

  if( !hent ) {
    throw network_error( "gethostbyname: " + std::string(hstrerror(local_h_errno)), false );
  }

#else
  hent = ::gethostbyname( host.c_str() );

  if( !hent ) {
    throw network_error( "gethostbyname" );
  }

#endif

  sa.sin_family = PF_INET;
  sa.sin_port = htons(port);
  memcpy( (char*)&sa.sin_addr, (char*)hent->h_addr, hent->h_length );
}


Inet_addr::Inet_addr( int port_ ):
  host("127.0.0.1"), port(port_)
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
