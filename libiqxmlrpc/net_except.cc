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
//  $Id: net_except.cc,v 1.5 2006-09-07 09:35:42 adedov Exp $

#include <string.h>
#include "sysinc.h"
#include "net_except.h"

namespace {

inline std::string
exception_message(const std::string& prefix, bool use_errno)
{
  std::string retval = prefix;

  if (use_errno)
  {
    retval += ": ";

    char buf[256];
    buf[255] = 0;
    char* b = buf;

#if not defined _WINDOWS && defined _GNU_SOURCE
    b = strerror_r( errno, buf, sizeof(buf) - 1 );
#elif not defined _WINDOWS
    strerror_r( errno, buf, sizeof(buf) - 1 );
#else
    strerror_s( buf, sizeof(buf) - 1, WSAGetLastError() );
#endif

    retval += std::string(b);
  }

  return retval;
}

}

iqnet::network_error::network_error( const std::string& msg, bool use_errno ):
  std::runtime_error( exception_message(msg, use_errno) )
{
}

// vim:ts=2:sw=2:et
