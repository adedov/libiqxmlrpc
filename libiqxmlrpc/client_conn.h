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

#ifndef _iqxmlrpc_client_conn_h_
#define _iqxmlrpc_client_conn_h_

#include "connection.h"
#include "http.h"
#include "request.h"
#include "response.h"

namespace iqxmlrpc {

class Client_options;

//! Transport independent base class for XML-RPC client's connection.
class LIBIQXMLRPC_API Client_connection {
public:
  Client_connection();
  virtual ~Client_connection();

  void set_options(const Client_options& o) { options = &o; }

  Response process_session(const Request&);

protected:
  http::Packet* read_response( const std::string& );
  virtual http::Packet* do_process_session( const std::string& ) = 0;

  const Client_options& opts() const { return *options; }

private:
  http::Packet_reader preader;
  const Client_options* options;

protected:
  unsigned read_buf_sz;
  char *read_buf;
};

//! Exception which be thrown by client when timeout occured.
class LIBIQXMLRPC_API Client_timeout: public iqxmlrpc::Exception {
public:
  Client_timeout():
    Exception( "Connection timeout." ) {}
};

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
