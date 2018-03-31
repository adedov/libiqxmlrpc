//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "client.h"
#include "client_conn.h"
#include "client_opts.h"

namespace iqxmlrpc {

//
// Client_base::Impl
//

class Client_base::Impl {
public:
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
    return client.get_connection();
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

int Client_base::timeout() const
{
  return impl_->opts.timeout();
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

void Client_base::set_traceinfo( const TraceInfo& traceInfo)
{
  impl_->opts.set_traceinfo(traceInfo);
}

Response Client_base::execute(
  const std::string& method, const Param_list& pl, const boost::optional<TraceInfo>& trace_info )
{
  Request req( method, pl );

  Auto_conn conn( *impl_.get(), *this );
  conn->set_options(impl_->opts);

  return conn->process_session( req, trace_info );
}

} // namespace iqxmlrpc

// vim:ts=2:sw=2:et
