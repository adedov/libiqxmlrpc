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
}


size_t Connection::send( const char* data, size_t len )
{
  return sock.send( data, len );
}


size_t Connection::recv( char* buf, size_t len )
{
  return sock.recv( buf, len );
}
