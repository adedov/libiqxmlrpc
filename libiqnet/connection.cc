#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "connection.h"
#include "net_except.h"

using namespace iqnet;


Connection::Connection( int s, const Inet_addr& addr ):
  sock(s), 
  peer_addr(addr)
{
}


Connection::~Connection()
{
  close( sock );
}


int Connection::send( const char* data, int len )
{
  int ret;
  
  if( (ret = ::send( sock, data, len, MSG_NOSIGNAL )) != len )
    throw network_error( "send" );

  return ret;
}


int Connection::recv( char* buf, int len )
{
  int ret;
  
  if( (ret = ::recv( sock, buf, len, MSG_NOSIGNAL )) == -1 )
    throw network_error( "recv" );

  return ret;
}


void Connection::send_str( const std::string& data )
{
  send( data.c_str(), data.length() );
}


const std::string Connection::recv_str()
{
  std::string data;
  unsigned pos = std::string::npos;

  for( pos = cache.find_first_of('\n'); 
       pos == std::string::npos; 
       pos = cache.find_first_of('\n') )
  {
    char buf[256];
    int  i = 0;

    switch( i = recv( buf, sizeof(buf) ) )
    {
      case 0:
        data = cache;
        return data;
      
      default:
        cache += std::string( buf, i );
    }
  }

  data = cache.substr( 0, pos );
  cache.erase( 0, pos+1 );
  
  if( data.length() && data[data.length()-1] == '\r' )
    data.erase( data.length()-1, 1 );

  return data;    
}


void Connection::set_non_blocking( bool flag )
{
  int enable = flag;
  if( ::setsockopt( sock, SOL_TCP, TCP_NODELAY, &enable, sizeof(int) ) == -1 )
    throw network_error( "setsockopt" );
}


bool Connection::get_non_blocking() const
{
  socklen_t len;
  int flag;
  
  ::getsockopt( sock, SOL_TCP, TCP_NODELAY, &flag, &len );
  return flag;
}
