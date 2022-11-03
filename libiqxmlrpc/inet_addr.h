//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_inet_addr_h_
#define _libiqnet_inet_addr_h_

#if _MSC_VER >= 1700
#include <winsock2.h>
#endif

#include "api_export.h"

#include <memory>
#include <string>

//! Object-oriented networking/multithreading infrastructure.
namespace iqnet
{

//! Returns host.domain of local processor.
std::string LIBIQXMLRPC_API get_host_name();

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif
//! An object representation of internet address.
/*! A wrapper for sockaddr_in system structure. */
class LIBIQXMLRPC_API Inet_addr {
  struct Impl;
  std::shared_ptr<Impl> impl_;

public:
  //! Does nothing.
  Inet_addr() {}

  Inet_addr( const struct sockaddr_in& );
  Inet_addr( const std::string& host, int port = 0 );
  Inet_addr( int port );

  virtual ~Inet_addr() {}

  const struct sockaddr_in* get_sockaddr() const;
  const std::string& get_host_name() const;
  int get_port() const;
};

} // namespace iqnet

#endif
