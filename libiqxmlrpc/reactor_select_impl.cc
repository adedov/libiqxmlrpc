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
//  $Id: reactor_select_impl.cc,v 1.4 2006-11-11 17:49:43 adedov Exp $

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
    int code = ::select( max_fd+1, &read_set, &write_set, &err_set, ptv );

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
