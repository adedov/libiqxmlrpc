#include <time.h>
#include <locale.h>
#include <ctype.h>
#include <iostream>
#include <functional>
#include <libiqxmlrpc/http.h>
#include <libiqxmlrpc/method.h>

using namespace iqxmlrpc::http;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
class Header::Option_eq: public std::unary_function<bool, Header::Option> {
  std::string name;
  
public:
  Option_eq( const std::string& n ): name(n) {}

  bool operator ()( const Header::Option& op )
  {
    return op.name == name;
  }
};
#endif

Header::Header()
{
  init_parser();
  
  set_option( "connection:", "close" );
}


Header::~Header()
{
}


void Header::set_version( const std::string& v )
{
  version_ = v;
}


void Header::set_content_length( unsigned lth )
{
  content_length_ = lth;
  std::ostringstream ss;
  ss << lth;
  
  set_option( "content-length:", ss.str() );
  
  if( lth )
    set_option( "content-type:", "text/xml" );
  else
    unset_option( "content-type:" );
}


inline void Header::init_parser()
{
  parsers["content-length:"] = Header::parse_content_length;
  parsers["content-type:"]   = Header::parse_content_type;
}


void Header::register_parser( const std::string& option, Parser parser )
{
  parsers[option] = parser;
}


void Header::parse( const std::string& s )
{
  std::istringstream ss( s );
  parse( ss );
}


void Header::parse( std::istringstream& ss )
{
  while( ss )
  {
    std::string word = read_option_name( ss );
    
    if( word.empty() )
    {
      read_eol(ss);
      break;
    }
    
    set_option( word, read_option_content(ss) );
  }
  
  for( Options_box::const_iterator i = options.begin(); i != options.end(); ++i )
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


void Header::set_option( const std::string& name, const std::string& value )
{
  Option_eq eq( name );
  Options_box::iterator i = std::find_if( options.begin(), options.end(), eq );
  
  if( i == options.end() )
  {
    options.push_back( Option(name, value) );
    return;
  }
  
  i->value = value;
}


void Header::unset_option( const std::string& name )
{
  Option_eq eq( name );
  Options_box::iterator i = std::find_if( options.begin(), options.end(), eq );

  if( i != options.end() )
    options.erase( i );
}


std::string Header::read_option_name( std::istringstream& ss )
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


void Header::read_eol( std::istringstream& ss )
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


std::string Header::read_option_content( std::istringstream& ss )
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


void Header::ignore_line( std::istringstream& ss )
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


std::string Header::dump() const
{
  std::ostringstream ss;
  
  for( Options_box::const_iterator i = options.begin(); i != options.end(); ++i )
    ss << i->name << " " << i->value << "\r\n";

  return ss.str() + "\r\n";
}


void Header::parse_content_type( Header* obj, std::istringstream& ss )
{
  std::string opt;
  ss >> opt;

  if( opt != "text/xml" )
    throw Unsupported_content_type();
}


void Header::parse_content_length( Header* obj, std::istringstream& ss )
{
  unsigned i;
  ss >> i;
  obj->set_content_length(i);
}


// ----------------------------------------------------------------------------
Request_header::Request_header( const std::string& rstr )
{
  std::istringstream ss( rstr );
  parse( ss );
}


Request_header::Request_header( std::istringstream& ss )
{
  parse( ss );
}


Request_header::Request_header( 
  const std::string& req_uri, 
  const std::string& client_host 
):
  uri_(req_uri),
  host_(client_host),
  user_agent_(PACKAGE " " VERSION)
{
  set_version( "HTTP/1.0" );
  set_option( "user-agent:", user_agent_ );
  set_option( "host:", host_ );
}


Request_header::~Request_header()
{
}


std::string Request_header::dump() const
{
  return "PUT " + uri() + " " + version() + "\r\n" + Header::dump();
}


void Request_header::parse( std::istringstream& ss )
{
  register_parser( "user-agent:", Request_header::parse_user_agent );
  register_parser( "host:", Request_header::parse_host );

  parse_method( ss );
  Header::parse( ss );
}


void Request_header::parse_method( std::istringstream& ss )
{
  std::istringstream rs( read_option_content(ss) );
  std::string method, version;

  rs >> method;
  if( method != "POST" )
    throw Method_not_allowed();

  rs >> uri_;
  rs >> version;
  set_version( version );
}


void Request_header::parse_host( Header* obj, std::istringstream& ss )
{
  Request_header* req = static_cast<Request_header*>(obj);
  ss >> req->host_;
}


void Request_header::parse_user_agent( Header* obj, std::istringstream& ss )
{
  Request_header* req = static_cast<Request_header*>(obj);
  ss >> req->user_agent_;
}


// ---------------------------------------------------------------------------
Response_header::Response_header( std::istringstream& ss )
{
  parse( ss );
}


Response_header::Response_header( const std::string& rstr )
{
  std::istringstream ss( rstr );
  parse( ss );
}


Response_header::Response_header( int c, const std::string& p ):
  code_(c),
  phrase_(p),
  server_(PACKAGE " " VERSION)
{
  set_version( "HTTP/1.1" );
  set_option( "date:", current_date() );
  set_option( "server:", server() );
}


Response_header::~Response_header()
{
}


void Response_header::parse( std::istringstream& ss )
{
  register_parser( "server:", Response_header::parse_server );
  
  std::string version;
  ss >> version;
  set_version( version );
  ss >> code_;
  phrase_ = read_option_content( ss );
  
  Header::parse( ss );
}


std::string Response_header::current_date() const
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


std::string Response_header::dump() const
{
  std::ostringstream ss;
  ss << version() << " " << code() <<  " " << phrase() << "\r\n";
  return ss.str() + Header::dump();
}


void Response_header::parse_server( Header* obj, std::istringstream& ss )
{
  Response_header* resp = static_cast<Response_header*>(obj);
  ss >> resp->server_;
}


// ---------------------------------------------------------------------------
Packet::Packet( Header* h, const std::string& co ):
  header_(h), 
  content_(co) 
{
  header_->set_content_length(content_.length());
}


Packet::Packet( const Packet& p ):
  header_(p.header_->clone()),
  content_(p.content_)
{
}


Packet::~Packet() 
{
  delete header_;
}


Packet& Packet::operator =( const Packet& p )
{
  delete header_;
  header_  = p.header_->clone();
  content_ = p.content_;
  
  return *this;
}


void Packet::set_keep_alive( bool keep_alive )
{
  if( keep_alive )
    header_->set_option( "connection:", "keep-alive" );
  else
    header_->set_option( "connection:", "close" );
}

  
// ---------------------------------------------------------------------------
Server::Server( Method_dispatcher* d ):
  iqxmlrpc::Server(d),
  packet(0),
  header(0)
{
}


Server::~Server()
{
  if( !packet )
    delete header;
  else
    delete packet;
}


bool Server::read_request( const std::string& s )
{
  if( !header )
    read_header(s);
  else
    content_cache += s;
  
  if( header && content_cache.length() >= header->content_length() )  
  {
    content_cache.erase( header->content_length(), std::string::npos );
    packet = new Packet( header, content_cache );
    content_cache = header_cache = "";
    return true;
  }
  
  return false;
}


Packet* Server::execute()
{
  iqxmlrpc::Response resp( iqxmlrpc::Server::execute(packet->content()) );
  std::string resp_str = resp.to_xml()->write_to_string_formatted();
  
  return new Packet( new Response_header(), resp_str );
}


void Server::read_header( const std::string& s )
{
  header_cache += s;
  unsigned i = header_cache.find( "\r\n\r\n" );
  
  if( i == std::string::npos )
    i = header_cache.find( "\n\n" );
  
  if( i == std::string::npos )
    return;
  
  std::istringstream ss( s );
  header = new Request_header( ss );
  
  for( char c = ss.get(); ss && !ss.eof(); c = ss.get() )
    content_cache += c;
}


// ---------------------------------------------------------------------------
std::string Client::do_execute( const Request& req )
{
  Request_header* req_h = new Request_header( uri_, host_ );
  Packet req_p( req_h, req.to_xml()->write_to_string_formatted() );
  send_request( req_p );
  
  Packet res_p( recv_response() );
  const Response_header* res_h = 
    static_cast<const Response_header*>(res_p.header());
  
  if( res_h->code() != 200 )
    throw Error_response( res_h->phrase(), res_h->code() );

  return res_p.content();
}
