//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "connection.h"
#include "net_except.h"

using namespace iqnet;


Connection::Connection( const Socket& s ):
  sock(s)
{
}


Connection::~Connection()
{
  ::shutdown( sock.get_handler(), 2 );
  sock.close();
}


void Connection::finish()
{
  sock.shutdown();
  sock.close();
}


int Connection::send( const char* data, int len )
{
  return sock.send( data, len );
}


int Connection::recv( char* buf, int len )
{
  return sock.recv( buf, len );
}
