//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "reactor_interrupter.h"
#include "connection.h"
#include "lock.h"
#include "socket.h"

#include <boost/optional.hpp>
#include <boost/thread/mutex.hpp>

#include <memory>

namespace iqnet {

class Interrupter_connection: public Connection {
public:
  Interrupter_connection(Reactor_base* r, const Socket& sock):
    Connection(sock), reactor_(r)
  {
    this->sock.set_non_blocking(true);
    reactor_->register_handler(this, Reactor_base::INPUT);
  }

  ~Interrupter_connection()
  {
    reactor_->unregister_handler(this);
  }

  bool is_stopper() const { return true; }

  void handle_input(bool& /* terminate */)
  {
    char nothing;
    recv(&nothing, 1);
  }

private:
  Reactor_base* reactor_;
};

class Reactor_interrupter::Impl: boost::noncopyable {
public:
  Impl(Reactor_base* reactor);
  ~Impl() {}

  void make_interrupt();

private:
  std::auto_ptr<Interrupter_connection> server_;
  Socket client_;
  boost::mutex lock_;
};


Reactor_interrupter::Impl::Impl(Reactor_base* reactor)
{
  Socket srv;
  srv.bind(Inet_addr("127.0.0.1", 0)); // bind to port 0, which means any port beyond 1024
  srv.listen(1);

  Inet_addr srv_addr(srv.get_addr());
  client_.connect( Inet_addr("127.0.0.1", srv_addr.get_port()) );
  Socket srv_conn(srv.accept());

  server_.reset(new Interrupter_connection(reactor, srv_conn));
}

void Reactor_interrupter::Impl::make_interrupt()
{
  boost::mutex::scoped_lock lk(lock_);
  client_.send("\0", 1);
}


Reactor_interrupter::Reactor_interrupter(Reactor_base* r):
  impl_(new Impl(r))
{
}

Reactor_interrupter::~Reactor_interrupter()
{
  delete impl_;
}

void Reactor_interrupter::make_interrupt()
{
  impl_->make_interrupt();
}

} // namespace iqnet
