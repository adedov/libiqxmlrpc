#ifndef _libiqxmlrpc_http_h_
#define _libiqxmlrpc_http_h_

#include <string>
#include <sstream>
#include <map>
#include <list>
#include <libiqxmlrpc/except.h>


namespace iqxmlrpc
{

//! Transport independent HTTP request, response, etc.
namespace http
{
  class Packet;
  class Request;
  class Response;
  
  class Error_response;
  class Bad_request;
  class Unsupported_content_type;
  class Method_not_allowed;
};


class http::Packet {
public:
  typedef void (*Parser)( Packet*, std::istringstream& );
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
  Options_box header;

  std::string version_;
  std::string content_;
  
public:
  Packet();
  Packet( const std::string& content );
  virtual ~Packet();
   
  const std::string& content() const { return content_; }
  const std::string& version() const { return version_; }
  
  void set_version( const std::string& v ) { version_ = v; }
  void add_option( const std::string& name, const std::string& value );
  
  virtual std::string dump() const;
  
protected:
  void register_parser( const std::string&, Parser );

  void parse( const std::string& );
  void parse( std::istringstream& );
  void read_eol( std::istringstream& );
  void ignore_line( std::istringstream& );
  std::string read_option_name( std::istringstream& );
  std::string read_option_content( std::istringstream& );

private:
  void init_parser();
  void parse_content( std::istringstream& );

  static void parse_content_type( Packet*, std::istringstream& );
  static void parse_content_length( Packet*, std::istringstream& );
};


class http::Request: public http::Packet {
  std::string uri_;
  std::string host_;
  std::string user_agent_;
  
public:
  Request( const std::string& request_string );
  Request( const std::string& uri, 
           const std::string& content, 
           const std::string& client_host );

  ~Request();

  const std::string& uri()   const { return uri_; }
  const std::string& host()  const { return host_; }
  const std::string& agent() const { return user_agent_; }

  std::string dump() const;

private:
  void parse_method( std::istringstream& );
  static void parse_host( Packet*, std::istringstream& );
  static void parse_user_agent( Packet*, std::istringstream& );
};


class http::Response: public http::Packet {
  int code_;
  std::string phrase_;
  std::string server_;

public:
  static Response incoming( const std::string& response_string );
  static Response outgoing( const std::string& content, 
                            int = 200, const std::string& = "OK" );

  ~Response();

  int                code()   const { return code_; }
  const std::string& phrase() const { return phrase_; }
  const std::string& server() const { return server_; }
  
  std::string dump() const;

protected:
  Response( const std::string& response_string );
  Response( const std::string& content, int code, const std::string& phrase );
  
private:
  std::string current_date() const;
};


class http::Error_response: public http::Response, public Exception {
public:
  Error_response( const std::string& phrase, int code ):
    Response( "", code, phrase ),
    Exception( "HTTP: " + phrase ) {}
  
  virtual ~Error_response() throw() {}
};


class http::Bad_request: public http::Error_response {
public:
  Bad_request():
    Error_response( "Bad request", 400 ) {}
};


class http::Unsupported_content_type: public http::Error_response {
public:
  Unsupported_content_type():
    Error_response( "Unsupported media type", 415 ) {}
};


class http::Method_not_allowed: public http::Error_response {
public:
  Method_not_allowed():
    Error_response( "Method not allowed", 405 )
  {
    add_option( "allowed:", "PUT" );
  }
};


};

#endif
