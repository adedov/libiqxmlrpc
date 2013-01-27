//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "acceptor.h"

#include "connection.h"
#include "conn_factory.h"
#include "firewall.h"
#include "inet_addr.h"
#include "net_except.h"

using namespace iqnet;


Acceptor::Acceptor( const iqnet::Inet_addr& bind_addr, Accepted_conn_factory* factory_, Reactor_base* reactor_ ):
  factory(factory_),
  reactor(reactor_),
  firewall(0)
{
  sock.bind( bind_addr );
  listen();
  reactor->register_handler( this, Reactor_base::INPUT );
}


Acceptor::~Acceptor()
{
  reactor->unregister_handler(this);
  sock.close();
}


void Acceptor::set_firewall( iqnet::Firewall_base* fw )
{
  delete firewall;
  firewall = fw;
}


void Acceptor::handle_input( bool& )
{
  accept();
}


inline void Acceptor::listen()
{
  sock.listen( 100 );
}


void Acceptor::accept()
{
  Socket new_sock( sock.accept() );

  if( firewall && !firewall->grant( new_sock.get_peer_addr() ) )
  {
    std::string msg = firewall->message();

    if (!msg.empty())
    {
      new_sock.send_shutdown(msg.c_str(), msg.size());
    } else {
      new_sock.shutdown();
    }

    return;
  }

  factory->create_accepted( new_sock );
}
