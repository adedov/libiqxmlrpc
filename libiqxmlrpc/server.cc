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
//  $Id: server.cc,v 1.25 2006-08-19 16:42:01 adedov Exp $

#include <memory>
#include "reactor.h"
#include "server.h"
#include "request.h"
#include "response.h"
#include "server_conn.h"

namespace iqxmlrpc {

Default_method_dispatcher::~Default_method_dispatcher()
{
  util::delete_ptrs( fs.begin(), fs.end(),
    util::Select2nd<Factory_map>());
}

void Default_method_dispatcher::register_method
  ( const std::string& name, Method_factory_base* fb )
{
  fs[name] = fb;
}

Method* Default_method_dispatcher::do_create_method(const std::string& name)
{
  if( fs.find(name) == fs.end() )
    return 0;

  return fs[name]->create();
}

// ---------------------------------------------------------------------------
Server::Server(
  int p,
  iqnet::Accepted_conn_factory* cf,
  Executor_factory_base* ef
):
  exec_factory(ef),
  port(p),
  reactor(ef->create_reactor()),
  conn_factory(cf),
  acceptor(0),
  firewall(0),
  exit_flag(false),
  soft_exit(false),
  log(0),
  max_req_sz(0),
  interceptors(0)
{
  default_disp = new Default_method_dispatcher;
  dispatchers.push_back(default_disp);
}


Server::~Server()
{
  util::delete_ptrs(dispatchers.begin(), dispatchers.end());
}


void Server::register_method(const std::string& name, Method_factory_base* f)
{
  default_disp->register_method(name, f);
}


void Server::perform_soft_exit()
{
  delete acceptor.release();
  soft_exit = true;
}


void Server::push_interceptor(Interceptor* ic)
{
  ic->nest(interceptors.release());
  interceptors.reset(ic);
}


void Server::push_dispatcher(Method_dispatcher_base* disp)
{
  dispatchers.push_back(disp);
}


void Server::enable_introspection()
{
  using iqxmlrpc::register_method;
  register_method<List_methods_m>(*this, "system.listMethods" );
  register_method<Method_signature_m>(*this, "system.methodSignature" );
  register_method<Method_help_m>(*this, "system.methodHelp" );
}


void Server::log_errors( std::ostream* log_ )
{
  log = log_;
}


void Server::set_max_request_sz( unsigned sz )
{
  max_req_sz = sz;
}


void Server::log_err_msg( const std::string& msg )
{
  if( log )
    *log << msg << std::endl;
}


Method* Server::create_method(const Method::Data& mdata)
{
  typedef DispatchersSet::iterator I;
  for (I i = dispatchers.begin(); i != dispatchers.end(); ++i)
  {
    Method* tmp = (*i)->create_method(mdata);
    if (tmp)
      return tmp;
  }

  throw Unknown_method(mdata.method_name);
}


void Server::schedule_execute( http::Packet* pkt, Server_connection* conn )
{
  Executor* executor = 0;

  try {
    std::auto_ptr<http::Packet> packet(pkt);
    std::auto_ptr<Request> req( parse_request(packet->content()) );

    Method::Data mdata = {
      req->get_name(),
      conn->get_peer_addr(),
      Server_feedback(this)
    };

    Method* meth = create_method( mdata );
    executor = exec_factory->create( meth, this, conn );
    executor->set_interceptors(interceptors.get());
    executor->execute( req->get_params() );
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

  std::auto_ptr<xmlpp::Document> xmldoc( resp.to_xml() );
  std::string resp_str = xmldoc->write_to_string_formatted( "utf-8" );
  
  http::Packet *packet = new http::Packet( new http::Response_header(), resp_str );
  conn->schedule_response( packet );
}


void Server::set_firewall( iqnet::Firewall_base* _firewall )
{
   firewall = _firewall;
}


void Server::work()
{
  if( !acceptor.get() )
  {
    acceptor.reset(new iqnet::Acceptor( port, conn_factory.get(), reactor.get()));
    acceptor->set_firewall( firewall );
  }

  try {
    for(bool have_handlers = true; have_handlers;)
    {
      if( exit_flag && !soft_exit )
        perform_soft_exit();

      have_handlers = reactor->handle_events();
    }
  }
  catch ( const iqnet::Reactor_base::No_handlers& )
  {
    // Soft exit performed.
  }
}

} // namespace iqxmlrpc
