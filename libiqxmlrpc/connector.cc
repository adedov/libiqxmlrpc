//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "connector.h"
#include "reactor_impl.h"

namespace iqnet {

#if defined(_WIN32)
#define IQXMLRPC_INPROGRESS WSAEWOULDBLOCK
#else
#define IQXMLRPC_INPROGRESS EINPROGRESS
#endif

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

    if (err && err != IQXMLRPC_INPROGRESS) {
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

  int ms = timeout > 0 ? timeout*1000 : -1;
  if (!reactor.handle_events(ms)) {
    connector.sock.close();
    throw iqxmlrpc::Client_timeout();
  }

  return create_connection( connector.sock );
}

} // namespace iqnet
