#include <time.h>
#include <locale.h>
#include <iostream>
#include <sstream>
#include <ctype.h>
#include <libiqxmlrpc/http.h>

using namespace iqxmlrpc::http;


Packet::Packet()
{
  init_parser();
}


Packet::Packet( const std::string& c ):
  content_(c)
{
  init_parser();

  std::ostringstream cl;
  cl << content_.length();
  add_option( "content-length:", cl.str() );
  
  if( !content_.empty() )
    add_option( "content-type:", "text/xml" );
}


Packet::~Packet()
{
}


inline void Packet::init_parser()
{
  parsers["content-length:"] = Packet::parse_content_length;
  parsers["content-type:"]   = Packet::parse_content_type;
}


void Packet::register_parser( const std::string& option, Parser parser )
{
  parsers[option] = parser;
}


void Packet::parse( const std::string& s )
{
  std::istringstream ss( s );
  parse( ss );
}


void Packet::parse( std::istringstream& ss )
{
  while( ss )
  {
    std::string word = read_option_name( ss );
    
    if( word.empty() )
    {
      parse_content( ss );
      break;
    }

    add_option( word, read_option_content(ss) );
  }
  
  for( Options_box::const_iterator i = header.begin(); i != header.end(); ++i )
  {
    Parsers_box::const_iterator j = parsers.find( i->name );

    if( j != parsers.end() )
    {
      std::istringstream os(i->value);
      j->second( this, os );
      continue;
    }
    
    if( i->name.find(":") == std::string::npos )
      throw Bad_request();
  }
}


void Packet::parse_content( std::istringstream& ss )
{
  read_eol( ss );
  
  for( char c = ss.get(); ss && !ss.eof(); c = ss.get() )
    content_ += c;
}


void Packet::add_option( const std::string& name, const std::string& value )
{
  header.push_back( Option(name, value) );
}


std::string Packet::read_option_name( std::istringstream& ss )
{
  std::string word;
  
  while( ss )
  {
    char c = ss.get();
    switch( c )
    {
      case ' ':
      case '\t':
        return word;
      
      case '\r':
      case '\n':
        ss.putback(c);
        return word;
      
      default:
        word += tolower(c);
    }
  }

  throw Bad_request();
}


void Packet::read_eol( std::istringstream& ss )
{
  char c = ss.get();
  switch( c )
  {
    case '\r':
      if( ss.get() != '\n' )
        throw Bad_request();
      break;
      
    case '\n':
      break;
    
    default:
      throw Bad_request();
  }
}


std::string Packet::read_option_content( std::istringstream& ss )
{
  char c = ' ';
  for( ; ss && (c == ' ' || c == '\t'); c = ss.get() );
  
  if( ss )
    ss.putback(c);
  else
    throw Bad_request();
  
  std::string option;
  while( ss )
  {
    if( ss.peek() == '\r' || ss.peek() == '\n' )
      break;
    
    option += ss.get();
  }

  read_eol( ss );
  return option;
}


void Packet::ignore_line( std::istringstream& ss )
{
  while( ss )
  {
    bool cr = false;
    
    switch( ss.get() )
    {
      case '\r':
        if( cr )
          throw Bad_request();
        cr = true;
        break;
      
      case '\n':
        return;
      
      default:
        if( cr )
          throw Bad_request();
    }
  }
}


std::string Packet::dump() const
{
  std::ostringstream ss;
  
  for( Options_box::const_iterator i = header.begin(); i != header.end(); ++i )
    ss << i->name << " " << i->value << "\r\n";

  return ss.str() + "\r\n" + content();
}


void Packet::parse_content_type( Packet* obj, std::istringstream& ss )
{
  std::string opt;
  ss >> opt;

  if( opt != "text/xml" )
    throw Unsupported_content_type();
}


void Packet::parse_content_length( Packet* obj, std::istringstream& ss )
{
  // ignore
}


// ----------------------------------------------------------------------------
Request::Request( const std::string& rstr )
{
  register_parser( "user-agent:", Request::parse_user_agent );
  register_parser( "host:", Request::parse_host );

  std::istringstream ss( rstr );
  parse_method( ss );
  parse( ss );
}


Request::Request( 
  const std::string& req_uri, 
  const std::string& content,  
  const std::string& client_host 
):
  Packet(content),
  uri_(req_uri),
  host_(client_host),
  user_agent_(PACKAGE " " VERSION)
{
  set_version( "HTTP/1.0" );
  add_option( "user-agent:", user_agent_ );
  add_option( "host:", host_ );
}


Request::~Request()
{
}


std::string Request::dump() const
{
  return "PUT " + uri() + " " + version() + "\r\n" + Packet::dump();
}


void Request::parse_method( std::istringstream& ss )
{
  std::istringstream rs( read_option_content(ss) );
  std::string method, version;

  rs >> method;
  if( method != "PUT" )
    throw Method_not_allowed();

  rs >> uri_;
  rs >> version;
  set_version( version );
}


void Request::parse_host( Packet* obj, std::istringstream& ss )
{
  Request* req = static_cast<Request*>(obj);
  ss >> req->host_;
}


void Request::parse_user_agent( Packet* obj, std::istringstream& ss )
{
  Request* req = static_cast<Request*>(obj);
  ss >> req->user_agent_;
}


// ---------------------------------------------------------------------------
Response Response::incoming( const std::string& s )
{
  return Response(s);
}


Response Response::outgoing( const std::string& ct, int c, const std::string& p )
{
  return Response( ct, c, p );
}


Response::Response( const std::string& rstr )
{
  std::istringstream ss( rstr );
  
  std::string version;
  ss >> version;
  set_version( version );
  ss >> code_;
  phrase_ = read_option_content( ss );
  
  parse( ss );
}


Response::Response( const std::string& ct, int c, const std::string& p ):
  Packet(ct),
  code_(c),
  phrase_(p),
  server_(PACKAGE " " VERSION)
{
  set_version( "HTTP/1.1" );
  add_option( "date:", current_date() );
  add_option( "server:", server() );
}


Response::~Response()
{
}


std::string Response::current_date() const
{
  time_t t;
  time( &t );
  struct tm* bdt = gmtime( &t );
  char *oldlocale = setlocale( LC_TIME, 0 );
  setlocale( LC_TIME, "C" );

  char date_str[30];
  date_str[29] = 0;
  strftime( date_str, 30, "%a, %d %b %Y %T %Z", bdt );
  
  setlocale( LC_TIME, oldlocale );
  return date_str;
}


std::string Response::dump() const
{
  std::ostringstream ss;
  ss << version() << " " << code() <<  " " << phrase() << "\r\n";
  
  return ss.str() + Packet::dump();
}
