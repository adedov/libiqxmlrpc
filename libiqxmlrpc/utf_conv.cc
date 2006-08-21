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
//  $Id: utf_conv.cc,v 1.6 2006-08-21 15:59:45 adedov Exp $

#include <errno.h>
#include "utf_conv.h"

using namespace iqxmlrpc;


iqxmlrpc::Utf_conv_base* iqxmlrpc::config::cs_conv = new Utf_null_conv;

void iqxmlrpc::config::set_encoding( const std::string& enc, unsigned m )
{
  delete iqxmlrpc::config::cs_conv;
  iqxmlrpc::config::cs_conv = new Utf_conv( enc, m );
}


// --------------------------------------------------------------------------


Utf_conv::Utf_conv( const std::string& enc, unsigned m ):
  max_sym_len(m)
{
  cd_to_utf = iconv_open( "utf-8", enc.c_str() );
  if( cd_to_utf == (iconv_t)-1 )
    throw Unknown_charset_conversion( enc + " to utf-8" );

  cd_from_utf = iconv_open( enc.c_str(), "utf-8" );
  if( cd_from_utf == (iconv_t)-1 )
    throw Unknown_charset_conversion( "utf-8 to " + enc );
}


Utf_conv::~Utf_conv()
{
  iconv_close( cd_to_utf );
  iconv_close( cd_from_utf );
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace 
{
  class auto_iconv_arg {
    char* p1;
    char* p2;
    
  public:
    auto_iconv_arg( char* p_ ): p1(p_), p2(p_) {}
    ~auto_iconv_arg()
    {
      delete[] p1;
    }  
  
    char* ptr1()
    {
      return p1;
    }
    
    char** ptr2()
    {
      return &p2;
    }    
  };
};
#endif

std::string Utf_conv::convert( iconv_t cd, const std::string& s )
{
  size_t ilen  = s.length();
  size_t olen  = ilen*max_sym_len;
  size_t ileft = ilen;
  size_t oleft = olen; 
  
#ifdef _WINDOWS || __CYGWIN__
  const char* ibuf = s.c_str();
#else
  char* ibuf = const_cast<char*>(s.c_str());
#endif

  auto_iconv_arg obuf( new char[olen] );

  int code = iconv( cd, &ibuf, &ileft, obuf.ptr2(), &oleft );
  if( code == -1 )
    throw Charset_conversion_failed();
    
  return std::string( obuf.ptr1(), olen - oleft );
}
