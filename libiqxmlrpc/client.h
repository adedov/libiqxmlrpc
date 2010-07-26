//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2007 Anton Dedov
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

#ifndef _iqxmlrpc_client_h_
#define _iqxmlrpc_client_h_

#include "connector.h"
#include "request.h"
#include "response.h"

#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>

namespace iqxmlrpc {

class Client_connection;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

//! Client base class.
//! It is responsible for performing RPC calls and connection management.
class LIBIQXMLRPC_API Client_base: boost::noncopyable {
public:
  Client_base(
    const iqnet::Inet_addr& addr,
    const std::string& uri,
    const std::string& vhost
  );

  virtual ~Client_base();

  //! Perform Remote Procedure Call
  Response execute( const std::string&, const Param_list& );

  //! Perform Remote Procedure Call with only one parameter transfered
  Response execute( const std::string& method, const Value& val )
  {
    Param_list pl;
    pl.push_back( val );
    return execute( method, pl );
  }

  //! Set address where actually connect to. <b>Tested with HTTP only.</b>
  void set_proxy(const iqnet::Inet_addr&);

  //! Set connection timeout
  /*! \param seconds TO value in seconds, negative number means infinity.
      \note It is not summary timeout.
      \note Timeout gives no effect on connection process.
  */
  void set_timeout( int seconds );

  //! Set connection keep-alive flag
  void set_keep_alive( bool keep_alive );

  //! Set data for HTTP Basic authentication
  void set_authinfo(const std::string& user, const std::string& password);

private:
  virtual void do_set_proxy( const iqnet::Inet_addr& ) = 0;
  virtual Client_connection* get_connection(bool non_blocking) = 0;

  friend class Auto_conn;
  class Impl;

  boost::scoped_ptr<Impl> impl_;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

//! Template of concrete client class.
//! It is responsible for conneciton establishment
//! (i.e. initialization of a concrete transport).
template <class TRANSPORT>
class Client: public iqxmlrpc::Client_base {
public:
  /*! \param addr Actual server address;
      \param uri  Requested URI (default "/RPC");
      \param host Requested virtual host (by default calculated form addr).
  */
  Client(
    const iqnet::Inet_addr& addr,
    const std::string& uri   = "/RPC",
    const std::string& vhost = ""
  ):
    Client_base(addr, uri, vhost),
    ctr(addr) {}

private:
  virtual void do_set_proxy(const iqnet::Inet_addr& addr)
  {
    proxy_ctr = iqnet::Connector<Proxy_connection>(addr);
  }

  virtual Client_connection* get_connection(bool non_blocking_flag)
  {
    if (proxy_ctr)
      return proxy_ctr->connect(non_blocking_flag);

    return ctr.connect(non_blocking_flag);
  }

  iqnet::Connector<TRANSPORT> ctr;

  typedef typename TRANSPORT::Proxy_connection Proxy_connection;
  boost::optional<iqnet::Connector<Proxy_connection> > proxy_ctr;
};

} // namespace iqxmlrpc

#endif
// vim:ts=2:sw=2:et
