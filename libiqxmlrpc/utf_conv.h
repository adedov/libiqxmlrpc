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
//  $Id: utf_conv.h,v 1.1 2004-10-03 16:23:02 adedov Exp $

#ifndef _libiqxmlrpc_utf_conv_h_
#define _libiqxmlrpc_utf_conv_h_

#include <string>
#include <stdexcept>
#include <iconv.h>


namespace iqxmlrpc 
{
  class Utf_conv;
};


//! Charset conversion utility class.
/*! Converts only from/to UTF-8. 
*/
class iqxmlrpc::Utf_conv {
public:
  class Unknown_conversion;
  class Conversion_failed;
  
private:
  iconv_t cd;
//  char unknown_sym;
    
public:
  //! Create instance for conversion from UTF-8 to another encoding.
  static Utf_conv* from_utf( const std::string& to );
  //! Create instance for conversion from some encoding to UTF-8.
  static Utf_conv* to_utf( const std::string& from );

  ~Utf_conv();

//  void set_unknown_sym( char c ) { unknown_sym = c; }
  
  //! Process conversion.
  /*! \param str string to convert
      \param max_sym_len points how much reserve memory for output string
  */
  std::string convert( const std::string& str, unsigned max_sym_len=3 );

private:
  Utf_conv( bool from_utf, const std::string& );
};


class iqxmlrpc::Utf_conv::Unknown_conversion: public std::runtime_error {
public:
  Unknown_conversion( const std::string& cs ):
    runtime_error( "iconv not aware about charset " + cs ) {}
};


class iqxmlrpc::Utf_conv::Conversion_failed: public std::runtime_error {
public:
  Conversion_failed():
    runtime_error( "Conversion failed." ) {}
};


#endif
