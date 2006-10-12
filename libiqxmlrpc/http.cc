//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2006 Anton Dedov
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
//  $Id: http.cc,v 1.31 2006-10-12 12:31:21 adedov Exp $

#include "sysinc.h"
#include <algorithm>
#include <deque>
#include <functional>
#include <memory>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include "http.h"
#include "method.h"

#ifdef _WINDOWS
#include "version.h"
#else
#include "../config.h"
#endif //_WINDOWS

namespace iqxmlrpc {
namespace http {

namespace names {
  const char crlf[]           = "\r\n";
  const char content_length[] = "content-length";
  const char content_type[]   = "content-type";
  const char connection[]     = "connection";
  const char host[]           = "host";
  const char user_agent[]     = "user-agent";
  const char server[]         = "server";
  const char date[]           = "date";
} // namespace names


namespace validator {

void unsigned_number(const std::string& val)
{
  const char errmsg[] = "bad format of numeric option";

  try {
    if (!boost::all(val, boost::is_digit()))
      throw Malformed_packet(errmsg);

    boost::lexical_cast<unsigned>(val);
  } catch (const boost::bad_lexical_cast&) {
    throw Malformed_packet(errmsg);
  }
}

void connection(const std::string& val)
{
  std::string tmp(val);
  boost::to_lower(tmp);

  if (tmp != "close" && tmp != "keep-alive")
    throw Malformed_packet("bad 'connection' option format");
}

void content_type(const std::string& val)
{
  std::string cont_type(val);
  boost::to_lower(cont_type);

  if (!boost::find_first(cont_type, "text/xml"))
    throw Unsupported_content_type(cont_type);
}

} // namespace validator


Header::Header(Verification_level lev):
  ver_level_(lev)
{
  set_conn_keep_alive(false);

  register_validator(names::content_length, validator::unsigned_number, HTTP_CHECK_WEAK);
  register_validator(names::content_type, validator::content_type, HTTP_CHECK_STRICT);
  register_validator(names::connection, validator::connection, HTTP_CHECK_WEAK);
}

Header::~Header()
{
}

void Header::register_validator(
  const std::string& name,
  Header::Option_validator_fn fn,
  Verification_level level)
{
  Option_validator v = { level, fn };
  validators_.insert(std::make_pair(name, v));
}

void Header::parse(const std::string& s)
{
  typedef std::deque<std::string> Tokens;
  Tokens lines;
  boost::split(lines, s, boost::is_any_of(names::crlf), boost::token_compress_on);

  if (!lines.empty()) {
    head_line_ = lines.front();
    lines.pop_front();
  }

  for (Tokens::iterator i = lines.begin(); i != lines.end(); ++i) {
    boost::iterator_range<std::string::iterator> j = boost::find_first(*i, ":");
    if (j.begin() == i->end())
      throw Malformed_packet("option line does not contain a colon symbol");

    std::string opt_name, opt_value;
    std::copy(i->begin(), j.begin(), std::back_inserter(opt_name));
    std::copy(j.end(), i->end(), std::back_inserter(opt_value));

    boost::trim(opt_name);
    boost::trim(opt_value);
    boost::to_lower(opt_name);
    set_option_checked(opt_name, opt_value);
  }
}

template <class T>
T Header::get_option(const std::string& name) const
{
  Options::const_iterator i = options_.find(name);

  if (i == options_.end()) {
      throw Malformed_packet("Missing mandatory header option '" + name + "'.");
  }

  try {
    return boost::lexical_cast<T>(i->second);
  } catch (boost::bad_lexical_cast&) {
    throw Malformed_packet("Header option '" + name + "' has wrong format.");
  }
}

std::string Header::get_string(const std::string& name) const
{
  return get_option<std::string>(name);
}

unsigned Header::get_unsigned(const std::string& name) const
{
  return get_option<unsigned>(name);
}

inline
void Header::set_option_checked(const std::string& name, const std::string& value)
{
  std::pair<Validators::const_iterator, Validators::const_iterator> v =
    validators_.equal_range(name);

  for (Validators::const_iterator i = v.first; i != v.second; ++i)
  {
    if (i->second.level <= ver_level_)
      i->second.fn(value);
  }

  set_option(name, value);
}

void Header::set_option(const std::string& name, const std::string& value)
{
  options_[name] = value;
}

void Header::set_option(const std::string& name, unsigned value)
{
  set_option(name, boost::lexical_cast<std::string>(value));
}

bool Header::option_exists(const std::string& name) const
{
  return options_.find(name) != options_.end();
}

void Header::set_option_default(const std::string& name, const std::string& value)
{
  if (option_exists(name))
    return;

  set_option(name, value);
}

std::string Header::dump() const
{
  std::string retval = dump_head();

  for (Options::const_iterator i = options_.begin(); i != options_.end(); ++i) {
    retval += i->first + ": " + i->second + names::crlf;
  }

  retval += names::crlf;
  return retval;
}

void Header::set_content_length(unsigned len)
{
  set_option(names::content_length, len);

  if (len)
    set_option(names::content_type, "text/xml");
}

void Header::set_conn_keep_alive(bool c)
{
  set_option(names::connection, c ? "keep-alive" : "close");
}

unsigned Header::content_length() const
{
  if (!option_exists(names::content_length))
    throw Length_required();

  return get_unsigned(names::content_length);
}

bool Header::conn_keep_alive() const
{
  return get_string(names::connection) == "keep-alive";
}

// ----------------------------------------------------------------------------
Request_header::Request_header(Verification_level lev, const std::string& to_parse):
  Header(lev)
{
  parse(to_parse);
  set_option_default(names::host, "");
  set_option_default(names::user_agent, "unknown");

  // parse method
  typedef std::deque<std::string> Token;
  Token method_line;
  boost::split(method_line, get_head_line(), boost::is_space(), boost::token_compress_on);

  if (method_line.empty())
    throw Bad_request();

  if( method_line[0] != "POST" )
    throw Method_not_allowed();

  if (method_line.size() > 1)
    uri_ = method_line[1];
}

Request_header::Request_header(
  const std::string& req_uri,
  const std::string& server_host
):
  uri_(req_uri)
{
  set_option(names::host, server_host);
  set_option(names::user_agent, PACKAGE " " VERSION);
}

std::string Request_header::dump_head() const
{
  return "POST " + uri() + " HTTP/1.0" + names::crlf;
}

std::string Request_header::host() const
{
  return get_string(names::host);
}

std::string Request_header::agent() const
{
  return get_string(names::user_agent);
}

// ---------------------------------------------------------------------------
Response_header::Response_header(Verification_level lev, const std::string& to_parse):
  Header(lev)
{
  parse(to_parse);
  set_option_default(names::server, "unknown");

  typedef std::deque<std::string> Token;
  Token resp_line;
  boost::split(resp_line, get_head_line(), boost::is_space(), boost::token_compress_on);

  if (resp_line.size() < 2) {
    throw Malformed_packet("Bad response");
  }

  try {
    code_ = boost::lexical_cast<int>(resp_line[1]);
  } catch (const boost::bad_lexical_cast&) {
    code_ = 0;
  }

  if (resp_line.size() > 2)
    phrase_ = resp_line[2];
}

Response_header::Response_header( int c, const std::string& p ):
  code_(c),
  phrase_(p)
{
  set_option(names::date, current_date());
  set_option(names::server, PACKAGE " " VERSION);
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

std::string Response_header::dump_head() const
{
  std::ostringstream ss;
  ss << "HTTP/1.1 " << code() <<  " " << phrase() << names::crlf;
  return ss.str();
}

std::string Response_header::server() const
{
  return get_string(names::server);
}

// ---------------------------------------------------------------------------
Packet::Packet( Header* h, const std::string& co ):
  header_(h),
  content_(co)
{
  header_->set_content_length(content_.length());
}

Packet::~Packet()
{
}

void Packet::set_keep_alive( bool keep_alive )
{
  header_->set_conn_keep_alive( keep_alive );
}

// ---------------------------------------------------------------------------
void Packet_reader::clear()
{
  header = 0;
  content_cache.erase();
  header_cache.erase();
  constructed = false;
  total_sz = 0;
}

void Packet_reader::check_sz( unsigned sz )
{
  if( !pkt_max_sz )
    return;

  if (header) {
    if (header->content_length() + header_cache.length() >= pkt_max_sz)
      throw Request_too_large();
  }

  if( (total_sz += sz) >= pkt_max_sz )
    throw Request_too_large();
}

bool Packet_reader::read_header( const std::string& s )
{
  using boost::iterator_range;
  using boost::find_first;

  header_cache += s;
  iterator_range<std::string::iterator> i = find_first(header_cache, "\r\n\r\n");

  if( i.begin() == i.end() )
    i = boost::find_first(header_cache, "\n\n");

  if( i.begin() == i.end() )
    return false;

  std::string tmp;
  std::copy(header_cache.begin(), i.begin(), std::back_inserter(tmp));
  std::copy(i.end(), header_cache.end(), std::back_inserter(content_cache));
  header_cache = tmp;
  return true;
}

} // namespace http
} // namespace iqxmlrpc
