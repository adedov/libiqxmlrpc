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
//  $Id: utf_conv.h,v 1.4 2004-10-11 11:02:50 maxim Exp $

#ifndef _libiqxmlrpc_utf_conv_h_
#define _libiqxmlrpc_utf_conv_h_

#include <string>
extern "C" {
#include <iconv.h>
}

#include "except.h"
#include "value.h"


namespace iqxmlrpc 
{
  class Utf_conv_base;
  class Utf_null_conv;
  class Utf_conv;
};


class iqxmlrpc::Utf_conv_base {
public:
  class Unknown_charset_conversion;
  class Charset_conversion_failed;
  
public:
  virtual ~Utf_conv_base() {}
 
  virtual std::string to_utf( const std::string& ) = 0;
  virtual std::string from_utf( const std::string& ) = 0;
};


class iqxmlrpc::Utf_null_conv: public iqxmlrpc::Utf_conv_base {
public:
  //! No conversion
  std::string to_utf( const std::string& s ) 
  {
    return s;
  }
  
  //! No conversion
  std::string from_utf( const std::string& s ) 
  {
    return s;
  }  
};


//! Charset conversion utility class.
/*! Converts only from/to UTF-8. 
*/
class iqxmlrpc::Utf_conv: public iqxmlrpc::Utf_conv_base {
  iconv_t cd_to_utf;
  iconv_t cd_from_utf;
  unsigned max_sym_len;
//  char unknown_sym;
    
public:
  Utf_conv( const std::string& enc_name, unsigned max_sym_len );
  ~Utf_conv();

//  void set_unknown_sym( char c ) { unknown_sym = c; }

  std::string to_utf( const std::string& s )
  {
    return convert( cd_to_utf, s );
  }
  
  std::string from_utf( const std::string& s )
  {
    return convert( cd_from_utf, s );
  }

private:
  std::string convert( iconv_t, const std::string& );
};


class iqxmlrpc::Utf_conv_base::Unknown_charset_conversion: 
  public iqxmlrpc::Exception 
{
public:
  Unknown_charset_conversion( const std::string& cs ):
    Exception( "iconv not aware about charset " + cs ) {}
};


class iqxmlrpc::Utf_conv_base::Charset_conversion_failed: 
  public iqxmlrpc::Exception 
{
public:
  Charset_conversion_failed():
    Exception( "Charset conversion failed." ) {}
};


#endif
