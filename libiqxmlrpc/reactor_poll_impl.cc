//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2004-2006 Anton Dedov
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
//  $Id: reactor_poll_impl.cc,v 1.4 2006-11-11 17:49:43 adedov Exp $

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
