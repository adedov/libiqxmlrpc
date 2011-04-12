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

#include "connector.h"
#include "reactor_impl.h"

namespace iqnet {

struct Connect_processor: public Event_handler {
  Reactor_base& reactor;
  Socket sock;

  Connect_processor(Reactor_base& r):
    reactor(r)
  {
    sock.set_non_blocking(true);
  }

  Socket::Handler get_handler() const
  {
    return sock.get_handler();
  }

  void handle_output( bool& terminate )
  {
    terminate = true;
    int err = sock.get_last_error();

    if (err) {
      sock.close();
      throw network_error("Connector", true, err);
    }
  }
};

Connector_base::Connector_base( const iqnet::Inet_addr& peer ):
  peer_addr(peer)
{
}

Connector_base::~Connector_base()
{
}

iqxmlrpc::Client_connection*
Connector_base::connect(int timeout)
{
  Reactor<Null_lock> reactor;
  Connect_processor connector(reactor);
  bool done = connector.sock.connect( peer_addr );

  if (done)
    return create_connection( connector.sock );

  reactor.register_handler( &connector, Reactor_base::OUTPUT );

  if (!reactor.handle_events(timeout*1000))
    throw iqxmlrpc::Client_timeout();

  return create_connection( connector.sock );
}

} // namespace iqnet
