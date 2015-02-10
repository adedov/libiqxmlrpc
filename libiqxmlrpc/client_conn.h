//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_client_conn_h_
#define _iqxmlrpc_client_conn_h_

#include <vector>
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
  http::Packet* read_response( const std::string&, bool read_hdr_only = false );
  virtual http::Packet* do_process_session( const std::string& ) = 0;

  const Client_options& opts() const { return *options; }

  char* read_buf() { return &read_buf_[0]; }
  size_t read_buf_sz() const { return read_buf_.size(); }

private:
  virtual std::string decorate_uri() const;

  http::Packet_reader preader;
  const Client_options* options;
  std::vector<char> read_buf_;
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
