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

#include "client_conn.h"
#include "client_opts.h"
#include "http.h"

#include <libxml++/libxml++.h>

namespace iqxmlrpc {

Client_connection::Client_connection():
  read_buf_sz(65000),
  read_buf(new char[read_buf_sz])
{
}

Client_connection::~Client_connection()
{
  delete[] read_buf;
}

Response Client_connection::process_session( const Request& req )
{
  using namespace http;

  try
  {
    std::string req_xml_str( req.dump_xml(false) );

    std::auto_ptr<Request_header> req_h(
      new Request_header(
        decorate_uri(),
        opts().vhost(),
        opts().addr().get_port() ));

    if (opts().has_authinfo())
      req_h->set_authinfo( opts().auth_user(), opts().auth_passwd() );

    Packet req_p( req_h.release(), req_xml_str );
    req_p.set_keep_alive( opts().keep_alive() );

    // Received packet
    std::auto_ptr<Packet> res_p( do_process_session(req_p.dump()) );

    const Response_header* res_h =
      static_cast<const Response_header*>(res_p->header());

    if( res_h->code() != 200 )
      throw Error_response( res_h->phrase(), res_h->code() );

    return parse_response( res_p->content() );
  }
  catch( const xmlpp::exception& e )
  {
    throw Parse_error( e.what() );
  }
}

http::Packet* Client_connection::read_response( const std::string& s, bool hdr_only )
{
  return preader.read_response( s, hdr_only );
}

std::string Client_connection::decorate_uri() const
{
  return opts().uri();
}

} // namespace iqxmlrpc

// vim:ts=2:sw=2:et
