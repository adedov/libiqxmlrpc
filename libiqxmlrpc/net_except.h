//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_net_except_h_
#define _libiqnet_net_except_h_

#include "api_export.h"

#include <stdexcept>
#include <string>

namespace iqnet
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4275)
#endif

//! Exception class to wrap a network's subsystem errors.
class LIBIQXMLRPC_API network_error: public std::runtime_error {
public:
  network_error( const std::string& msg, bool use_errno = true, int myerrno = 0 );
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqnet

#endif
