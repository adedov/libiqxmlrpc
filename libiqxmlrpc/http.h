//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqxmlrpc_http_h_
#define _libiqxmlrpc_http_h_

#include "except.h"
#include "inet_addr.h"
#include "xheaders.h"

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>

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
  bool      expect_continue() const;

  void set_content_length( size_t ln );
  void set_conn_keep_alive( bool );
  void set_option(const std::string& name, const std::string& value);

  void get_xheaders(iqxmlrpc::XHeaders& xheaders) const;
  void set_xheaders(const iqxmlrpc::XHeaders& xheaders);

  //! Return text representation of header including final CRLF.
  std::string dump() const;

protected:
  bool option_exists(const std::string&) const;
  void set_option_default(const std::string& name, const std::string& value);
  void set_option_default(const std::string& name, unsigned value);
  void set_option_checked(const std::string& name, const std::string& value);
  void set_option(const std::string& name, size_t value);

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
  size_t pkt_max_sz;
  size_t total_sz;
  bool continue_sent_;

public:
  Packet_reader():
    header(0),
    constructed(false),
    pkt_max_sz(0),
    total_sz(0),
    continue_sent_(false)
  {
  }

  ~Packet_reader()
  {
    if( !constructed )
      delete header;
  }

  void set_verification_level(Verification_level lev)
  {
    ver_level_ = lev;
  }

  void set_max_size( size_t m )
  {
    pkt_max_sz = m;
  }

  bool expect_continue() const;
  Packet* read_request( const std::string& );
  Packet* read_response( const std::string&, bool read_header_only );
  void set_continue_sent(); 

private:
  void clear();
  void check_sz( size_t );
  bool read_header( const std::string& );

  template <class Header_type>
  Packet* read_packet( const std::string&, bool = false );
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

  const Response_header* response_header() const
  {
    return dynamic_cast<const Response_header*>(header());
  }

  // deprecated
  std::string dump_error_response() const { return dump(); }
};

} // namespace http
} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
