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

#ifndef _libiqxmlrpc_http_h_
#define _libiqxmlrpc_http_h_

#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "api_export.h"
#include "except.h"
#include "inet_addr.h"

namespace iqxmlrpc {

class Auth_Plugin_base;

//! XML-RPC HTTP transport-independent infrastructure.
/*! Contains classes which responsible for transport-indepenent
    HTTP collaboration functionality. Such as packet parsing/constructing,
    wrapping XML-RPC message into HTTP-layer one and vice versa.
*/
namespace http {

//! The level of HTTP sanity checks.
enum Verification_level { HTTP_CHECK_WEAK, HTTP_CHECK_STRICT };

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

//! HTTP header. Responsible for parsing,
//! creating generic HTTP headers.
class LIBIQXMLRPC_API Header {
public:
  Header(Verification_level = HTTP_CHECK_WEAK);
  virtual ~Header();

  unsigned  content_length()  const;
  bool      conn_keep_alive() const;

  void set_content_length( unsigned ln );
  void set_conn_keep_alive( bool );
  void set_option(const std::string& name, const std::string& value);

  //! Return text representation of header including final CRLF.
  std::string dump() const;

protected:
  bool option_exists(const std::string&) const;
  void set_option_default(const std::string& name, const std::string& value);
  void set_option_default(const std::string& name, unsigned value);
  void set_option_checked(const std::string& name, const std::string& value);
  void set_option(const std::string& name, unsigned value);

  const std::string&  get_head_line() const { return head_line_; }
  std::string         get_string(const std::string& name) const;
  unsigned            get_unsigned(const std::string& name) const;

  //
  // Parser interface
  //

  typedef boost::function<void (const std::string&)> Option_validator_fn;

  void register_validator(
    const std::string&,
    Option_validator_fn,
    Verification_level);

  void parse(const std::string&);

private:
  template <class T>
  T get_option(const std::string& name) const;

  virtual std::string dump_head() const = 0;

private:
  struct Option_validator {
    Verification_level level;
    Option_validator_fn fn;
  };

  typedef std::map<std::string, std::string> Options;
  typedef std::multimap<std::string, Option_validator> Validators;

  std::string head_line_;
  Options options_;
  Validators validators_;
  Verification_level ver_level_;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

//! HTTP request's header.
class LIBIQXMLRPC_API Request_header: public Header {
  std::string uri_;

public:
  Request_header( Verification_level, const std::string& to_parse );
  Request_header( const std::string& uri, const std::string& vhost, int port );

  const std::string& uri() const { return uri_; }
  std::string host()  const;
  std::string agent() const;
 
  bool has_authinfo() const; 
  void get_authinfo(std::string& user, std::string& password) const;
  void set_authinfo(const std::string& user, const std::string& password);

private:
  virtual std::string dump_head() const;
};

//! HTTP response's header.
class LIBIQXMLRPC_API Response_header: public Header {
  int code_;
  std::string phrase_;

public:
  Response_header( Verification_level, const std::string& to_parse );
  Response_header( int = 200, const std::string& = "OK" );

  int code() const { return code_; }
  const std::string& phrase() const { return phrase_; }
  std::string server() const;

private:
  std::string current_date() const;
  virtual std::string dump_head() const;
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

//! HTTP packet: Header + Content.
class LIBIQXMLRPC_API Packet {
protected:
  boost::shared_ptr<http::Header> header_;
  std::string content_;

public:
  Packet( http::Header* header, const std::string& content );
  virtual ~Packet();

  //! Sets header option "connection: {keep-alive|close}".
  //! By default connection is close.
  void set_keep_alive( bool = true );

  const http::Header* header()  const { return header_.get(); }
  const std::string&  content() const { return content_; }

  std::string dump() const
  {
    return header_->dump() + content_;
  }
};

#ifdef _MSC_VER
#pragma warning(pop)
#pragma warning(disable: 4251)
#endif

//! Helper that responsible for constructing HTTP packets of specified type
//! (request or response).
class Packet_reader {
  std::string header_cache;
  std::string content_cache;
  Header* header;
  Verification_level ver_level_;
  bool constructed;
  unsigned pkt_max_sz;
  unsigned total_sz;

public:
  Packet_reader():
    header(0),
    constructed(false),
    pkt_max_sz(0),
    total_sz(0) {}

  ~Packet_reader()
  {
    if( !constructed )
      delete header;
  }

  void set_verification_level(Verification_level lev)
  {
    ver_level_ = lev;
  }

  void set_max_size( unsigned m )
  {
    pkt_max_sz = m;
  }

  Packet* read_request( const std::string& );
  Packet* read_response( const std::string& );

private:
  void clear();
  void check_sz( unsigned );
  bool read_header( const std::string& );

  template <class Header_type>
  Packet* read_packet( const std::string& );
};


//! Exception which is thrown on syntax error during HTTP packet parsing.
class LIBIQXMLRPC_API Malformed_packet: public Exception {
public:
  Malformed_packet():
    Exception( "Malformed HTTP packet received.") {}

  Malformed_packet(const std::string& problem_domain):
    Exception( "Malformed HTTP packet received (" + problem_domain + ")." ) {}
};

//! Exception related to HTTP protocol.
//! Can be sent as error response to client.
class LIBIQXMLRPC_API Error_response: public Packet, public Exception {
public:
  Error_response( const std::string& phrase, int code ):
    Packet( new Response_header(code, phrase), "" ),
    Exception( "HTTP: " + phrase ) {}

  ~Error_response() throw() {};

  std::string dump_error_response() const { return dump(); }
};

} // namespace http
} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
