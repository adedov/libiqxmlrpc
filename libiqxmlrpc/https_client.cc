//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2007 Anton Dedov
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

#include <boost/lexical_cast.hpp>
#include "client_opts.h"
#include "https_client.h"
#include "reactor_impl.h"

using namespace iqnet;

namespace iqxmlrpc {

//
// Proxy_request_header
//

class LIBIQXMLRPC_API Proxy_request_header: public http::Header {
public:
  Proxy_request_header( const iqnet::Inet_addr& connect_addr ):
    addr_(connect_addr) {}

private:
  virtual std::string dump_head() const
  {
    return
      "CONNECT "
      + addr_.get_host_name() + ":"
      + boost::lexical_cast<std::string>(addr_.get_port())
      + " HTTP/1.0\r\n";
  }

  const iqnet::Inet_addr& addr_;
};

//
// Https_proxy_client_connection
//

Https_proxy_client_connection::Https_proxy_client_connection(
  const iqnet::Socket& s,
  bool nb
):
  Client_connection(),
  Connection( s ),
  reactor( new Reactor<Null_lock> ),
  resp_packet(0),
  non_blocking(nb)
{
  sock.set_non_blocking( nb );
}

http::Packet* Https_proxy_client_connection::do_process_session( const std::string& s )
{
  setup_tunnel();

  Https_client_connection https_conn(sock, non_blocking);
  https_conn.post_connect();

  return https_conn.do_process_session(s);
}

void Https_proxy_client_connection::setup_tunnel()
{
  reactor->register_handler( this, Reactor_base::OUTPUT );

  Proxy_request_header h(opts().addr());
  out_str = h.dump();

  do {
    int to = opts().timeout() >= 0 ? opts().timeout() * 1000 : -1;
    if( !reactor->handle_events(to) )
      throw Client_timeout();
  }
  while( !resp_packet );

  const http::Response_header* res_h =
    static_cast<const http::Response_header*>(resp_packet->header());

  if( res_h->code() != 200 )
    throw http::Error_response( res_h->phrase(), res_h->code() );
}

void Https_proxy_client_connection::handle_output( bool& )
{
  int sz = send( out_str.c_str(), out_str.length() );
  out_str.erase( 0, sz );

  if( out_str.empty() )
  {
    reactor->unregister_handler( this, Reactor_base::OUTPUT );
    reactor->register_handler( this, Reactor_base::INPUT );
  }
}

void Https_proxy_client_connection::handle_input( bool& )
{
  for( unsigned sz = read_buf_sz; (sz == read_buf_sz) && !resp_packet ; )
  {
    read_buf[0] = 0;

    if( !(sz = recv( read_buf, read_buf_sz )) )
      throw iqnet::network_error( "Connection closed by peer.", false );

    resp_packet.reset( read_response(std::string(read_buf, sz), true) );
  }

  if( resp_packet )
  {
    reactor->unregister_handler( this );
  }
}

//
// Https_client_connection
//

Https_client_connection::Https_client_connection( const iqnet::Socket& s, bool nb ):
  Client_connection(),
  iqnet::ssl::Reaction_connection( s ),
  reactor( new Reactor<Null_lock> ),
  resp_packet(0),
  established(false)
{
  iqnet::ssl::Reaction_connection::sock.set_non_blocking( nb );
}


inline void Https_client_connection::reg_send_request()
{
  reg_send( out_str.c_str(), out_str.length() );
}


http::Packet* Https_client_connection::do_process_session( const std::string& s )
{
  out_str = s;
  resp_packet = 0;

  if( established )
    reg_send_request();

  do {
    int to = opts().timeout() >= 0 ? opts().timeout() * 1000 : -1;
    if( !reactor->handle_events(to) )
      throw Client_timeout();
  }
  while( !resp_packet );

  return resp_packet;
}


void Https_client_connection::connect_succeed()
{
  established = true;
  reg_send_request();
}


void Https_client_connection::send_succeed( bool& )
{
  read_buf[0] = 0;
  reg_recv( read_buf, read_buf_sz );
}


void Https_client_connection::recv_succeed( bool&, int req_len, int sz )
{
  if( !sz )
    throw iqnet::network_error( "Connection closed by peer.", false );

  std::string s( read_buf, sz );
  resp_packet = read_response( s );

  if( !resp_packet )
  {
    read_buf[0] = 0;
    reg_recv( read_buf, read_buf_sz );
  }
}

} // namespace iqxmlrpc

// vim:ts=2:sw=2:et
