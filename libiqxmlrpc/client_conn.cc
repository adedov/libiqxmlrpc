//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "client_conn.h"
#include "client_opts.h"
#include "http.h"

#include <memory>

namespace iqxmlrpc {

Client_connection::Client_connection():
  read_buf_(65536, '\0')
{
}

Client_connection::~Client_connection()
{
}

Response Client_connection::process_session( const Request& req, const XHeaders& xheaders )
{
  using namespace http;

  std::string req_xml_str( dump_request(req) );

  std::unique_ptr<Request_header> req_h(
    new Request_header(
      decorate_uri(),
      opts().vhost(),
      opts().addr().get_port() ));

  if (opts().has_authinfo())
    req_h->set_authinfo( opts().auth_user(), opts().auth_passwd() );

  req_h->set_xheaders( opts().xheaders() );
  req_h->set_xheaders( xheaders );

  Packet req_p( req_h.release(), req_xml_str );
  req_p.set_keep_alive( opts().keep_alive() );

  // Received packet
  std::unique_ptr<Packet> res_p( do_process_session(req_p.dump()) );

  const Response_header* res_h =
    static_cast<const Response_header*>(res_p->header());

  if( res_h->code() != 200 )
    throw Error_response( res_h->phrase(), res_h->code() );

  return parse_response( res_p->content() );
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
