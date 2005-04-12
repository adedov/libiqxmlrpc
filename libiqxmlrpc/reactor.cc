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
//  $Id: reactor.cc,v 1.10 2005-04-12 17:17:08 bada Exp $

#include <vector>
#include <list>
#include <deque>
#include <functional>
#include <algorithm>

#ifndef _WINDOWS
  #include "../config.h"
#endif

#include "sysinc.h"

#ifndef HAVE_POLL
  #define POLLIN      0x0001    /* There is data to read */
  #define POLLPRI     0x0002    /* There is urgent data to read */
  #define POLLOUT     0x0004    /* Writing now will not block */
  #define POLLERR     0x0008    /* Error condition */
  #define POLLHUP     0x0010    /* Hung up */
  #define POLLNVAL    0x0020    /* Invalid request: fd not open */
#endif

#include "reactor.h"
#include "net_except.h"

using namespace iqnet;


#ifndef DOXYGEN_SHOULD_SKIP_THIS
class Reactor::Reactor_impl {
public:
  typedef Socket::Handler S_fd;

  struct Handler {
    S_fd            fd;
    Event_handler*  handler;
    short           mask;
    short           revents;
    
    Handler():
      fd(0), handler(0), mask(0), revents(0) {}
    
    Handler( S_fd fd_, Event_handler* eh, Reactor::Event_mask m ):
      fd(fd_), handler(eh), mask(m), revents(0) {}
  };
  
  class Handler_fd_eq: public std::unary_function<bool, Handler> {
    S_fd fd;
  public:
    Handler_fd_eq( S_fd fd_ ): fd(fd_) {}
  
    bool operator ()( const Handler& h )
    {
      return fd == h.fd;
    }
  };

  typedef std::list<Handler> Handlers_box;
  typedef Handlers_box::const_iterator const_iterator;
  typedef Handlers_box::iterator iterator;

#ifdef HAVE_POLL
  typedef std::vector<struct pollfd> Pollfd_vec;
#endif

private:
  iqnet::Lock *lock;
  Handlers_box handlers;
  Handlers_box called_by_user;

#ifdef HAVE_POLL
  Pollfd_vec pfd;
#else
  S_fd max_fd;
  fd_set read_set, write_set, err_set;
#endif
  
public:
  Reactor_impl( iqnet::Lock* lck ): lock(lck) {}

  ~Reactor_impl()
  {
    delete lock;
  }

  unsigned       size()  const { return handlers.size(); }
  const_iterator begin() const { return handlers.begin(); }
  iterator       begin()       { return handlers.begin(); }
  const_iterator end()   const { return handlers.end(); }
  iterator       end()         { return handlers.end(); }
  
  void register_handler( Event_handler*, Event_mask );
  void unregister_handler( Event_handler*, Event_mask );
  void unregister_handler( Event_handler* );  
  void fake_event( Event_handler*, Event_mask );

  bool handle_events( Reactor::Timeout );
  
private:
  iterator find_handler( Event_handler* );
  void prepare_user_events();
  void prepare_system_events();
  void invoke_event_handler( Handler& );
  void invoke_clients_handler( Handler&, bool& );
  void invoke_servers_handler( Handler&, bool& );
  void handle_user_events();
  bool handle_system_events( Reactor::Timeout );
};
#endif


inline Reactor::Reactor_impl::iterator 
  Reactor::Reactor_impl::find_handler( Event_handler* eh )
{
  S_fd fd = eh->get_handler();
  return std::find_if( begin(), end(), Handler_fd_eq(fd) );
}


void Reactor::Reactor_impl::register_handler( Event_handler* eh, Event_mask mask )
{
  iterator i = find_handler( eh );
  Auto_lock a( lock );
  
  if( i == end() )
    handlers.push_back( Handler( eh->get_handler(), eh, mask ) );
  else
    i->mask |= mask;
}


void Reactor::Reactor_impl::unregister_handler( Event_handler* eh, Event_mask mask )
{
  iterator i = find_handler( eh );
  
  if( i != end() )
  {
    Auto_lock a( lock );
    int newmask = (i->mask &= !mask);

    if( !newmask )
      handlers.erase(i);
  }
}


void Reactor::Reactor_impl::unregister_handler( Event_handler* eh )
{
  iterator i = find_handler( eh );
  
  if( i != end() )
  {
    Auto_lock a( lock );
    handlers.erase(i);
  }
}


void Reactor::Reactor_impl::fake_event( Event_handler* eh, Event_mask mask )
{
  iterator i = find_handler( eh );

  if( i == end() )
    return;

  Auto_lock a( lock );
  i->revents |= mask;
}


void Reactor::Reactor_impl::prepare_user_events()
{
  called_by_user.clear();

  for( iterator i = begin(); i != end(); ++i )
  {
    if( i->revents && (i->mask | i->revents) )
    {
      called_by_user.push_back( *i );
      Auto_lock a( lock );  
      i->revents &= !i->mask;
    }
  }
}


void Reactor::Reactor_impl::prepare_system_events()
{
#ifdef HAVE_POLL
  pfd.clear();
#else
  max_fd = 0;
  FD_ZERO( &read_set );
  FD_ZERO( &write_set );
  FD_ZERO( &err_set );
#endif
    
  for( const_iterator i = begin(); i != end(); ++i )
  {
#ifdef HAVE_POLL    
    short events = i->mask & INPUT ? POLLIN|POLLPRI : 0;
    events |= i->mask & OUTPUT ? POLLOUT : 0;
    struct pollfd sfd = { i->fd, events, 0 };
    pfd.push_back( sfd );
#else
    if( i->mask )
      FD_SET( i->fd, &err_set );
    if( i->mask & INPUT )
      FD_SET( i->fd, &read_set );
    if( i->mask & OUTPUT )
      FD_SET( i->fd, &write_set );
    
    max_fd = i->fd > max_fd ? i->fd : max_fd;
#endif    
  }
}


inline
void Reactor::Reactor_impl::invoke_clients_handler( Handler& h, bool& terminate )
{
  bool in  = h.revents & POLLIN;
  bool out = h.revents & POLLOUT || h.revents & POLLPRI;
  bool err = h.revents & POLLERR;
  bool hup = h.revents & POLLHUP;
  
  if( in && out )
    h.handler->handle_io( terminate );
  else if( in )
    h.handler->handle_input( terminate );
  else if( out )
    h.handler->handle_output( terminate );
  else if( err )
    h.handler->handle_error( terminate );
  else if( hup )
    terminate = true;
}


void Reactor::Reactor_impl::invoke_servers_handler( Handler& h, bool& terminate )
{
  try {
    invoke_clients_handler( h, terminate );
  }
  catch( const std::exception& e )
  {
    h.handler->log_exception( e );
    terminate = true;
  }
  catch( ... )
  {
    h.handler->log_unknown_exception();
    terminate = true;
  }
}


void Reactor::Reactor_impl::invoke_event_handler( Handler& h )
{
  bool terminate = false;

  if( h.handler->catch_in_reactor() )
    invoke_servers_handler( h, terminate );
  else
    invoke_clients_handler( h, terminate );
  
  if( terminate )
  {
    unregister_handler( h.handler );
    h.handler->finish();
  }
}


void Reactor::Reactor_impl::handle_user_events()
{
  prepare_user_events();
  
  while( !called_by_user.empty() )
  {
    Handler h = called_by_user.front();
    called_by_user.pop_front();
    invoke_event_handler( h );
  }  
}

  
bool Reactor::Reactor_impl::handle_system_events( Reactor::Timeout to_ms )
{
  if( begin() == end() )
    throw Reactor::No_handlers();
  
  prepare_system_events();
  unsigned hsz = size();
  int code = 0;
  
#ifdef HAVE_POLL
  code = poll( &pfd[0], hsz, to_ms );
#else
  struct timeval tv;
  struct timeval *ptv = 0;
    
  if( to_ms >= 0 )
  {
    tv.tv_sec = 0;
    tv.tv_usec = to_ms * 1000;
    ptv = &tv;
  }

  code = select( max_fd+1, &read_set, &write_set, &err_set, ptv );
#endif

  if( code < 0 )
    throw network_error( "poll/select" );
  
  if( !code )
    return false;

  std::deque<Handler> active_hs;
  
#ifdef HAVE_POLL    
  for( unsigned i = 0; i < hsz; i++ )
  {
    if( pfd[i].revents )
    {
      const_iterator j = std::find_if( begin(), end(), Handler_fd_eq(pfd[i].fd) );
      Handler h( *j );
      h.revents = pfd[i].revents;
      active_hs.push_back( h );
    }
  }
#else
  for( iterator i = begin(); i != end(); ++i )
  {
    short revents = 0;
    revents |= FD_ISSET( i->fd, &read_set ) ? POLLIN : 0;
    revents |= FD_ISSET( i->fd, &write_set ) ? POLLOUT : 0;
    revents |= FD_ISSET( i->fd, &err_set ) ? POLLERR : 0;
    
    if( revents )
    {
      Handler h( *i );
      h.revents = revents;
      active_hs.push_back( h );
    }
  }
#endif
  
  while( !active_hs.empty() )
  {
    Handler h = active_hs.front();
    active_hs.pop_front();
    invoke_event_handler( h );
  }
  
  return true;
}


bool Reactor::Reactor_impl::handle_events( Reactor::Timeout to_ms )
{
  handle_user_events();
  return handle_system_events( to_ms );
}


// ---------------------------------------------------------------------------
Reactor::Reactor( iqnet::Lock* lock_ ):
  impl(new Reactor_impl(lock_))
{
}


Reactor::~Reactor()
{
  delete impl;
}


void Reactor::register_handler( Event_handler* eh, Event_mask mask )
{
  impl->register_handler( eh, mask );
}


void Reactor::unregister_handler( Event_handler* eh, Event_mask mask )
{
  impl->unregister_handler( eh, mask );
}


void Reactor::unregister_handler( Event_handler* eh )
{
  impl->unregister_handler( eh );
}


void Reactor::fake_event( Event_handler* eh, Event_mask mask )
{
  impl->fake_event( eh, mask );
}


bool Reactor::handle_events( Reactor::Timeout to_ms )
{
  if( impl->begin() == impl->end() )
    throw No_handlers();

  return impl->handle_events( to_ms );
}
