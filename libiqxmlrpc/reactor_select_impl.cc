//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#include "config.h"

#ifndef HAVE_POLL
#include "reactor_select_impl.h"

using namespace iqnet;

typedef Reactor_base::HandlerStateList HandlerStateList;

Reactor_select_impl::Reactor_select_impl()
{
}

Reactor_select_impl::~Reactor_select_impl()
{
}

void Reactor_select_impl::reset(const HandlerStateList& in)
{
  hs = in;
  max_fd = 0;
  FD_ZERO( &read_set );
  FD_ZERO( &write_set );
  FD_ZERO( &err_set );

  for( HandlerStateList::const_iterator i = in.begin(); i != in.end(); ++i )
  {
    if( i->mask & Reactor_base::INPUT )
      FD_SET( i->fd, &read_set );
    if( i->mask & Reactor_base::OUTPUT )
      FD_SET( i->fd, &write_set );

    max_fd = i->fd > max_fd ? i->fd : max_fd;
  }
}

bool Reactor_select_impl::poll(HandlerStateList& out, Reactor_base::Timeout to_ms)
{
  struct timeval tv;
  struct timeval *ptv = 0;

  if( to_ms >= 0 )
  {
    tv.tv_sec = 0;
    tv.tv_usec = to_ms * 1000;
    ptv = &tv;
  }

  do {
    int code = ::select( static_cast<int>(max_fd+1), &read_set, &write_set, &err_set, ptv );

    if( code < 0 )
    {
      if (errno == EINTR)
        continue;

      throw network_error( "select()" );
    }

    if( !code )
      return false;

  } while (false);

  for( HandlerStateList::const_iterator i = hs.begin(); i != hs.end(); ++i )
  {
    short revents = 0;
    revents |= FD_ISSET( i->fd, &read_set ) ? Reactor_base::INPUT : 0;
    revents |= FD_ISSET( i->fd, &write_set ) ? Reactor_base::OUTPUT : 0;
//    revents |= FD_ISSET( i->fd, &err_set ) ? POLLERR : 0;

    if( revents )
    {
      Reactor_base::HandlerState h( i->fd );
      h.revents = revents;
      out.push_back( h );
    }
  }

  return true;
}

#endif // not defined HAVE_POLL
