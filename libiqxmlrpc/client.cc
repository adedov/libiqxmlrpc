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
//  $Id: client.cc,v 1.8 2006-08-30 18:01:36 adedov Exp $

#include <libxml++/libxml++.h>
#include "client.h"
#include "http.h"

using namespace iqxmlrpc;


Client_connection::Client_connection():
  read_buf_sz(1024),
  read_buf(new char[read_buf_sz]),
  keep_alive(false)
{
}


Client_connection::~Client_connection()
{
  delete[] read_buf;
}


Response Client_connection::process_session( 
  const Request& req, const std::string& uri, const std::string& vhost )
{
  using namespace http;

  try 
  {
    std::auto_ptr<xmlpp::Document> xmldoc( req.to_xml() );
    std::string req_xml_str( xmldoc->write_to_string_formatted( "utf-8" ) );
    Packet req_p( new Request_header( uri, vhost ), req_xml_str );
    req_p.set_keep_alive( keep_alive );
  
    // Received packet
    std::auto_ptr<Packet> res_p( do_process_session(req_p.dump()) );
    
    const Response_header* res_h = 
      static_cast<const Response_header*>(res_p->header());
    
    if( res_h->code() != 200 )
      throw Error_response( res_h->phrase(), res_h->code() );
  
    xmlpp::DomParser parser;
    parser.set_substitute_entities();
    parser.parse_memory( res_p->content() );
  
    return Response( parser.get_document() );
  }
  catch( const xmlpp::exception& e )
  {
    throw Parse_error( e.what() );
  }
}


http::Packet* Client_connection::read_response( const std::string& s )
{
  return preader.read_packet( s );
}
