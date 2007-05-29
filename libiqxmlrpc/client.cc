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

#include "client.h"
#include "client_conn.h"
#include "client_opts.h"

namespace iqxmlrpc {

//
// Client_base::Impl
//

struct Client_base::Impl {
  Impl(
    const iqnet::Inet_addr& addr,
    const std::string& uri,
    const std::string& vhost
  ):
    opts(addr, uri, vhost) {}

  Client_options opts;
  boost::scoped_ptr<Client_connection> conn_cache;
};

//
// Auto_conn
//

class Auto_conn: boost::noncopyable {
public:
  Auto_conn( Client_base::Impl& client_impl, Client_base& client ):
    client_impl_(client_impl)
  {
    if (opts().keep_alive())
    {
      if (!cimpl().conn_cache)
        cimpl().conn_cache.reset( create_connection(client) );
      
      conn_ptr_ = cimpl().conn_cache.get();

    } else {
      tmp_conn_.reset( create_connection(client) );
      conn_ptr_ = tmp_conn_.get();
    }
  }

  ~Auto_conn()
  {
    if (!cimpl().opts.keep_alive())
      cimpl().conn_cache.reset();
  }

  Client_connection* operator ->()
  {
    return conn_ptr_;
  }

private:
  Client_connection* create_connection(Client_base& client)
  {
    return client.get_connection(opts().non_blocking());
  }

  const Client_options& opts()
  { 
    return client_impl_.opts;
  }
  
  Client_base::Impl& cimpl()
  {
    return client_impl_;
  }

  Client_base::Impl& client_impl_;
  boost::scoped_ptr<Client_connection> tmp_conn_;
  Client_connection* conn_ptr_;
};

//
// Client_base
//

Client_base::Client_base(
  const iqnet::Inet_addr& addr,
  const std::string& uri,
  const std::string& vhost
):
  impl_(new Impl(addr, uri, vhost))
{
}

Client_base::~Client_base()
{
}

void Client_base::set_proxy( const iqnet::Inet_addr& addr )
{
  do_set_proxy( addr );
}

void Client_base::set_timeout( int seconds )
{
  impl_->opts.set_timeout(seconds);
}

//! Set connection keep-alive flag
void Client_base::set_keep_alive( bool keep_alive )
{
  impl_->opts.set_keep_alive(keep_alive);

  if (!keep_alive && impl_->conn_cache)
    impl_->conn_cache.reset();
}

void Client_base::set_authinfo( const std::string& u, const std::string& p )
{
  impl_->opts.set_authinfo( u, p );
}

Response Client_base::execute(
  const std::string& method, const Param_list& pl )
{
  Request req( method, pl );

  Auto_conn conn( *impl_.get(), *this );
  conn->set_options(impl_->opts);

  return conn->process_session( req );
}

} // namespace iqxmlrpc

// vim:ts=2:sw=2:et
