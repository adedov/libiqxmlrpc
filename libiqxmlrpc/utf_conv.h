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
//  $Id: utf_conv.h,v 1.6 2006-09-04 12:13:31 adedov Exp $

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

  //! Library's run-time configuration specific stuff
  namespace config
  {
    //! Current charset converter
    extern iqxmlrpc::Utf_conv_base* cs_conv;

    //! Change current charset converter
    void set_encoding( const std::string&, unsigned max_sym_len = 3 );
  };
};


//! Base class for charset conversion utilities.
class iqxmlrpc::Utf_conv_base {
public:
  virtual ~Utf_conv_base() {}

  //! Convert specified string to UTF-8
  virtual std::string to_utf( const std::string& ) = 0;

  //! Convert UTF-8 string to some application specific encoding
  virtual std::string from_utf( const std::string& ) = 0;
};


//! No charset conversion class.
class iqxmlrpc::Utf_null_conv: public iqxmlrpc::Utf_conv_base {
public:
  //! Returns string without any conversion
  std::string to_utf( const std::string& s )
  {
    return s;
  }

  //! Returns string without any conversion
  std::string from_utf( const std::string& s )
  {
    return s;
  }
};


//! Charset conversion utility class.
/*! Converts strings to/from UTF-8 using GNU iconv. */
class iqxmlrpc::Utf_conv: public iqxmlrpc::Utf_conv_base {
public:
  class Unknown_charset_conversion;
  class Charset_conversion_failed;

private:
  iconv_t cd_to_utf;
  iconv_t cd_from_utf;
  unsigned max_sym_len;
//  char unknown_sym;

public:
  /*! \param enc_name application specific encoding
      \param max_sym_len maximum number of bytes to represent one symbol
                         of enc_name's encoding in UTF-8
  */
  Utf_conv( const std::string& enc_name, unsigned max_sym_len );
  ~Utf_conv();

//  void set_unknown_sym( char c ) { unknown_sym = c; }

  //! Convert specified string to UTF-8
  std::string to_utf( const std::string& s )
  {
    return convert( cd_to_utf, s );
  }

  //! Convert specified string from UTF-8 to specific encoding
  std::string from_utf( const std::string& s )
  {
    return convert( cd_from_utf, s );
  }

private:
  std::string convert( iconv_t, const std::string& );
};


//! No such conversion exception class
class iqxmlrpc::Utf_conv::Unknown_charset_conversion:
  public iqxmlrpc::Exception
{
public:
  Unknown_charset_conversion( const std::string& cs ):
    Exception( "iconv not aware about charset " + cs ) {}
};


//! Conversion fault exception class
class iqxmlrpc::Utf_conv::Charset_conversion_failed:
  public iqxmlrpc::Exception
{
public:
  Charset_conversion_failed():
    Exception( "Charset conversion failed." ) {}
};


#endif
