//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#if _MSC_VER >= 1700
#include <windows.h>
#endif

#include <string.h>
#include "net_except.h"

namespace {

inline std::string
exception_message(const std::string& prefix, bool use_errno, int myerrno)
{
  std::string retval = prefix;

  if (use_errno)
  {
    retval += ": ";

    char buf[256];
    buf[255] = 0;
    char* b = buf;

    myerrno = myerrno ? myerrno : errno;

#if !defined _WIN32 && defined _GNU_SOURCE
    b = strerror_r( myerrno, buf, sizeof(buf) - 1 );
#elif !defined _WIN32
    strerror_r( myerrno, buf, sizeof(buf) - 1 );
#else
    strerror_s( buf, sizeof(buf) - 1, WSAGetLastError() );
#endif

    retval += std::string(b);
  }

  return retval;
}

}

iqnet::network_error::network_error( const std::string& msg, bool use_errno, int myerrno ):
  std::runtime_error( exception_message(msg, use_errno, myerrno) )
{
}

// vim:ts=2:sw=2:et
