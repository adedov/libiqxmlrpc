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

#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <libxml++/libxml++.h>
#include <memory>

#include "server.h"
#include "auth_plugin.h"
#include "http_errors.h"
#include "reactor.h"
#include "reactor_interrupter.h"
#include "request.h"
#include "response.h"
#include "server_conn.h"

namespace iqxmlrpc {

struct Server::Impl {
  Executor_factory_base* exec_factory;

  iqnet::Inet_addr bind_addr;

  std::auto_ptr<iqnet::Reactor_base>          reactor;
  std::auto_ptr<iqnet::Reactor_interrupter>   interrupter;
  std::auto_ptr<iqnet::Accepted_conn_factory> conn_factory;
  std::auto_ptr<iqnet::Acceptor>              acceptor;
  iqnet::Firewall_base* firewall;

  bool exit_flag;
  std::ostream* log;
  unsigned max_req_sz;
  http::Verification_level ver_level;

  Method_dispatcher_manager  disp_manager;
  std::auto_ptr<Interceptor> interceptors;
  const Auth_Plugin_base*    auth_plugin;

  Impl(
    const iqnet::Inet_addr& addr,
    iqnet::Accepted_conn_factory* cf,
    Executor_factory_base* ef):
      exec_factory(ef),
      bind_addr(addr),
      reactor(ef->create_reactor()),
      interrupter(new iqnet::Reactor_interrupter(reactor.get())),
      conn_factory(cf),
      acceptor(0),
      firewall(0),
      exit_flag(false),
      log(0),
      max_req_sz(0),
      ver_level(http::HTTP_CHECK_WEAK),
      interceptors(0),
      auth_plugin(0)
  {
  }
};

// ---------------------------------------------------------------------------
Server::Server(
  const iqnet::Inet_addr& addr,
  iqnet::Accepted_conn_factory* cf,
  Executor_factory_base* ef):
    impl(new Server::Impl(addr, cf, ef))
{
}

Server::~Server()
{
  delete impl;
}

void Server::register_method(const std::string& name, Method_factory_base* f)
{
  impl->disp_manager.register_method(name, f);
}

void Server::set_exit_flag()
{
  impl->exit_flag = true;
  interrupt();
}

void Server::interrupt()
{
  impl->interrupter->make_interrupt();
}

iqnet::Reactor_base* Server::get_reactor()
{
  return impl->reactor.get();
}

void Server::push_interceptor(Interceptor* ic)
{
  ic->nest(impl->interceptors.release());
  impl->interceptors.reset(ic);
}

void Server::push_dispatcher(Method_dispatcher_base* disp)
{
  impl->disp_manager.push_back(disp);
}

void Server::enable_introspection()
{
  impl->disp_manager.enable_introspection();
}

void Server::log_errors( std::ostream* log_ )
{
  impl->log = log_;
}

void Server::set_max_request_sz( unsigned sz )
{
  impl->max_req_sz = sz;
}

unsigned Server::get_max_request_sz() const
{
  return impl->max_req_sz;
}

void Server::set_verification_level( http::Verification_level lev )
{
  impl->ver_level = lev;
}

http::Verification_level Server::get_verification_level() const
{
  return impl->ver_level;
}

void Server::set_auth_plugin( const Auth_Plugin_base& ap )
{
  impl->auth_plugin = &ap;
}

void Server::log_err_msg( const std::string& msg )
{
  if( impl->log )
    *impl->log << msg << std::endl;
}

namespace {

boost::optional<std::string>
authenticate(const http::Packet& pkt, const Auth_Plugin_base* ap)
{
  using namespace http;

  if (!ap)
    return boost::optional<std::string>();

  const Request_header& hdr =
    dynamic_cast<const Request_header&>(*pkt.header());

  if (!hdr.has_authinfo())
  {
    if (!ap->authenticate_anonymous())
      throw Unauthorized();

    return boost::optional<std::string>();
  }

  std::string username, password;
  hdr.get_authinfo(username, password);

  if (!ap->authenticate(username, password))
    throw Unauthorized();

  return username;
}

} // anonymous namespace

void Server::schedule_execute( http::Packet* pkt, Server_connection* conn )
{
  using boost::scoped_ptr;
  using boost::optional;

  Executor* executor = 0;

  try {
    scoped_ptr<http::Packet> packet(pkt);
    optional<std::string> authname = authenticate(*pkt, impl->auth_plugin);
    scoped_ptr<Request> req( parse_request(packet->content()) );

    Method::Data mdata = {
      req->get_name(),
      conn->get_peer_addr(),
      Server_feedback(this)
    };

    Method* meth = impl->disp_manager.create_method( mdata );

    if (authname)
      meth->authname(authname.get());

    executor = impl->exec_factory->create( meth, this, conn );
    executor->set_interceptors(impl->interceptors.get());
    executor->execute( req->get_params() );
  }
  catch( const iqxmlrpc::http::Error_response& e )
  {
    log_err_msg( e.what() );
    std::auto_ptr<Executor> executor_to_delete(executor);
    http::Packet *pkt = new http::Packet(e);
    conn->schedule_response( pkt );
  }
  catch( const iqxmlrpc::Exception& e )
  {
    log_err_msg( std::string("Server: ") + e.what() );
    Response err_r( e.code(), e.what() );
    schedule_response( err_r, conn, executor );
  }
  catch( const std::exception& e )
  {
    log_err_msg( std::string("Server: ") + e.what() );
    Response err_r( -32500 /*application error*/, e.what() );
    schedule_response( err_r, conn, executor );
  }
  catch( ... )
  {
    log_err_msg( "Server: Unknown exception" );
    Response err_r( -32500  /*application error*/, "Unknown Error" );
    schedule_response( err_r, conn, executor );
  }
}

void Server::schedule_response(
  const Response& resp, Server_connection* conn, Executor* exec )
{
  std::auto_ptr<Executor> executor_to_delete(exec);
  std::string resp_str = resp.dump_xml(false);
  http::Packet *packet = new http::Packet(new http::Response_header(), resp_str);
  conn->schedule_response( packet );
}

void Server::set_firewall( iqnet::Firewall_base* _firewall )
{
   impl->firewall = _firewall;
}

void Server::work()
{
  if( !impl->acceptor.get() )
  {
    impl->acceptor.reset(new iqnet::Acceptor( impl->bind_addr, get_conn_factory(), get_reactor()));
    impl->acceptor->set_firewall( impl->firewall );
  }

  for(bool have_handlers = true; have_handlers;)
  {
    if (impl->exit_flag)
      break;

    have_handlers = get_reactor()->handle_events();
  }

  impl->acceptor.reset(0);
  impl->exit_flag = false;
}

iqnet::Accepted_conn_factory* Server::get_conn_factory()
{
  return impl->conn_factory.get();
}

} // namespace iqxmlrpc

// vim:ts=2:sw=2:et
