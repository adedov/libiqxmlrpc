#ifndef _libiqxmlrpc_http_h_
#define _libiqxmlrpc_http_h_

#include <string>
#include <sstream>
#include <map>
#include <list>
#include <libiqxmlrpc/except.h>
#include <libiqxmlrpc/transport.h>


namespace iqxmlrpc
{

//! XML-RPC HTTP infrastructure.
namespace http
{
  class Header;
  class Request_header;
  class Response_header;
  class Packet;
  
  class Error_response;
  class Bad_request;
  class Method_not_allowed;
  class Unsupported_content_type;
    
  class Server;
};


//! HTTP header. Responsible for parsing, 
//! creating generic HTTP headers.
class http::Header {
public:
  typedef void (*Parser)( Header*, std::istringstream& );
  typedef std::map<std::string, Parser> Parsers_box;

  struct Option {
    std::string name;
    std::string value;
    
    Option( const std::string& n, const std::string& v ):
      name(n), value(v) {}
  };

  typedef std::list<Option> Options_box;

private:
  Parsers_box parsers;
  Options_box options;

  std::string version_;
  unsigned    content_length_;
  
public:
  Header();
  virtual ~Header();

  virtual Header* clone() const { return new Header(*this); }
  
  const std::string& version()        const { return version_; }
  unsigned           content_length() const { return content_length_; }
  
  void set_version( const std::string& v );
  void set_content_length( unsigned ln );

  //! Adds/alters header option.
  /*! For example: 
      \code set_option( "allow:", "PUT" ); \endcode
  */
  void set_option( const std::string& name, const std::string& value );
  
  //! Remove option from header.
  void unset_option( const std::string& name );

  //! Return text representation of header including final CRLF.
  virtual std::string dump() const;
  
protected:
  //! Register parser for specific option name.
  void register_parser( const std::string&, Parser );

  void parse( const std::string& );
  void parse( std::istringstream& );

  void read_eol( std::istringstream& );
  void ignore_line( std::istringstream& );
  std::string read_option_name( std::istringstream& );
  std::string read_option_content( std::istringstream& );

private:
  void init_parser();

  static void parse_content_type( Header*, std::istringstream& );
  static void parse_content_length( Header*, std::istringstream& );
};


//! HTTP request's header.
class http::Request_header: public http::Header {
  std::string uri_;
  std::string host_;
  std::string user_agent_;
  
public:
  Request_header( const std::string& to_parse );
  Request_header( std::istringstream& to_parse );
  Request_header( const std::string& uri, const std::string& client_host );

  ~Request_header();

  Request_header* clone() const { return new Request_header(*this); }

  const std::string& uri()   const { return uri_; }
  const std::string& host()  const { return host_; }
  const std::string& agent() const { return user_agent_; }

  std::string dump() const;

private:
  void parse( std::istringstream& );
  void parse_method( std::istringstream& );
  static void parse_host( Header*, std::istringstream& );
  static void parse_user_agent( Header*, std::istringstream& );
};


//! HTTP response's header.
class http::Response_header: public http::Header {
  int code_;
  std::string phrase_;
  std::string server_;

public:
  Response_header( const std::string& to_parse );
  Response_header( std::istringstream& to_parse );
  Response_header( int = 200, const std::string& = "OK" );

  ~Response_header();

  Response_header* clone() const { return new Response_header(*this); }

  int                code()   const { return code_; }
  const std::string& phrase() const { return phrase_; }
  const std::string& server() const { return server_; }
  
  std::string dump() const;

private:
  void parse( std::istringstream& );
  std::string current_date() const;

  static void parse_server( Header*, std::istringstream& );
};


//! HTTP packet: Header + Content.
class http::Packet {
protected:
  http::Header* header_;
  std::string content_;
  
public:
  Packet( const Packet& );
  Packet( http::Header* header, const std::string& content );
  virtual ~Packet();

  Packet& operator =( const Packet& );

  //! Sets header option "connection: {keep-alive|close}".
  //! By default connection is close.
  void set_keep_alive( bool = true );
  
  const http::Header* header()  const { return header_; }
  const std::string&  content() const { return content_; }

  virtual std::string dump() const
  {
    return header_->dump() + content_;
  }  
};


//! HTTP XML-RPC server independet from 
//! low level transport implementation.
class http::Server: private iqxmlrpc::Server {
  std::string header_cache;
  std::string content_cache;
  
  http::Header* header;
  http::Packet* packet;

public:
  Server( Method_dispatcher* );
  ~Server();

  //! Considers parameter as a piece of request.
  //! \return true if whole request has been read,
  //! \return false if more data is needed.
  bool read_request( const std::string& );

  //! Executes request which has been read.
  http::Packet* execute();

private:
  void read_header( const std::string& );
};


//! Exception related to HTTP protocol. 
//! Can be sent as error response to client.
class http::Error_response: public http::Packet, public Exception {
public:
  Error_response( const std::string& phrase, int code ):
    Packet( new Response_header(code, phrase), "" ),
    Exception( "HTTP: " + phrase ) {}
  
  virtual ~Error_response() throw() {}
};


//! HTTP/1.1 400 Bad request
class http::Bad_request: public http::Error_response {
public:
  Bad_request():
    Error_response( "Bad request", 400 ) {}
};


//! HTTP/1.1 405 Method not allowed
class http::Method_not_allowed: public http::Error_response {
public:
  Method_not_allowed():
    Error_response( "Method not allowed", 405 )
  {
    header_->set_option( "allowed:", "PUT" );
  }
};


//! HTTP/1.1 415 Unsupported media type
class http::Unsupported_content_type: public http::Error_response {
public:
  Unsupported_content_type():
    Error_response( "Unsupported media type", 415 ) {}
};

};

#endif
