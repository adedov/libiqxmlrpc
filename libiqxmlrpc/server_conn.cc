//  Libiqnet + Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004 Anton Dedov
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
//  
//  $Id: server_conn.cc,v 1.1 2005-09-20 16:02:59 bada Exp $

#include "server_conn.h"
#include "server.h"

using namespace iqxmlrpc;

Server_connection::Server_connection( const iqnet::Inet_addr& a ):
  peer_addr(a),
  server(0),
  read_buf_sz(1024),
  read_buf(new char[1024]),
  keep_alive(false)
{
}


Server_connection::~Server_connection()
{
  delete[] read_buf;
}


void Server_connection::set_read_sz( unsigned rsz )
{
  delete[] read_buf;
  read_buf_sz = rsz;
  read_buf = new char[read_buf_sz];
}


http::Packet* Server_connection::read_request( const std::string& s )
{
  try 
  {
    preader.set_max_size( server->get_max_request_sz() );
    http::Packet* r = preader.read_packet(s);
    
    if( r )
      keep_alive = r->header()->conn_keep_alive();
    
    return r;
  }
  catch( const http::Malformed_packet& )
  {
    throw http::Bad_request();
  }
}


void Server_connection::schedule_response( http::Packet* pkt )
{
  std::auto_ptr<http::Packet> p(pkt);
  p->set_keep_alive( keep_alive );
  response = p->dump();
}
