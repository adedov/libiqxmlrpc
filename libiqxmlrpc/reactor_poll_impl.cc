//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "config.h"

#ifdef HAVE_POLL
#include "reactor_poll_impl.h"

#include <deque>
#include <sys/poll.h>
#include <vector>

using namespace iqnet;

typedef Reactor_base::HandlerStateList HandlerStateList;

struct Reactor_poll_impl::Impl {
  typedef std::vector<struct pollfd> Pollfd_vec;
  Pollfd_vec pfd;
};

Reactor_poll_impl::Reactor_poll_impl():
  impl(new Impl)
{
}

Reactor_poll_impl::~Reactor_poll_impl()
{
  delete impl;
}

void Reactor_poll_impl::reset(const HandlerStateList& in)
{
  impl->pfd.clear();

  for( HandlerStateList::const_iterator i = in.begin(); i != in.end(); ++i )
  {
    short events = i->mask & Reactor_base::INPUT ? POLLIN : 0;
    events |= i->mask & Reactor_base::OUTPUT ? POLLOUT : 0;
    struct pollfd sfd = { i->fd, events, 0 };
    impl->pfd.push_back( sfd );
  }
}

bool Reactor_poll_impl::poll(HandlerStateList& out, Reactor_base::Timeout to_ms)
{
  do {
    int code = ::poll( &impl->pfd[0], impl->pfd.size(), to_ms );

    if( code < 0 )
    {
      if (errno == EINTR)
        continue;

      throw network_error( "poll()" );
    }

    if( !code )
      return false;

  } while (false);

  for( unsigned i = 0; i < impl->pfd.size(); i++ )
  {
    if( impl->pfd[i].revents )
    {
      Reactor_base::HandlerState hs(impl->pfd[i].fd);
      hs.revents |= impl->pfd[i].revents & POLLIN  ? Reactor_base::INPUT : 0;
      hs.revents |= impl->pfd[i].revents & POLLOUT ? Reactor_base::OUTPUT : 0;
      hs.revents |= impl->pfd[i].revents & POLLERR ? Reactor_base::OUTPUT : 0;
      hs.revents |= impl->pfd[i].revents & POLLHUP ? Reactor_base::OUTPUT : 0;
      out.push_back( hs );
    }
  }

  return true;
}

#endif // HAVE_POLL
