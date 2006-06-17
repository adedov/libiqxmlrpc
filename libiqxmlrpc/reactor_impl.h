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
//  $Id: reactor_impl.h,v 1.3 2006-06-17 14:52:04 bada Exp $

#ifndef _iqxmlrpc_reactor_impl_h_
#define _iqxmlrpc_reactor_impl_h_

#include <assert.h>
#include <map>
#include <boost/utility.hpp>
#include "reactor.h"
#include "config.h"

#ifdef HAVE_POLL
#include "reactor_poll_impl.h"
  namespace iqnet 
  {
    typedef Reactor_poll_impl ReactorImpl;
  }
#else
#include "reactor_select_impl.h"
  namespace iqnet 
  {
    typedef Reactor_select_impl ReactorImpl;
  }
#endif // HAVE_POLL

namespace iqnet
{

//! The Reactor template class.
//! Lock param can be either boost::mutex or iqnet::Null_lock.
template <class Lock>
class Reactor: public Reactor_base, boost::noncopyable {
public:
  Reactor();
  ~Reactor() {}

  void register_handler( Event_handler*, Event_mask );
  void unregister_handler( Event_handler*, Event_mask );
  void unregister_handler( Event_handler* );

  void fake_event( Event_handler*, Event_mask );

  bool handle_events( Timeout ms = -1 );

private:
  typedef typename Lock::scoped_lock        scoped_lock;
  typedef std::map<Socket::Handler, Event_handler*> EventHandlersMap;
  typedef EventHandlersMap::iterator        h_iterator;
  typedef HandlerStateList::const_iterator  hs_const_iterator;
  typedef HandlerStateList::iterator        hs_iterator;

  unsigned          size()  const { return handlers_states.size(); }
  hs_const_iterator begin() const { return handlers_states.begin(); }
  hs_iterator       begin()       { return handlers_states.begin(); }
  hs_const_iterator end()   const { return handlers_states.end(); }
  hs_iterator       end()         { return handlers_states.end(); }
 
  Event_handler* find_handler(Socket::Handler);
  hs_iterator find_handler_state(Event_handler*);

  void handle_user_events();
  bool handle_system_events( Timeout );
  
  void invoke_clients_handler( Event_handler*, HandlerState&, bool& terminate );
  void invoke_servers_handler( Event_handler*, HandlerState&, bool& terminate );
  void invoke_event_handler( HandlerState& );

private:
  Lock lock;
  ReactorImpl impl;

  EventHandlersMap handlers;
  HandlerStateList handlers_states;
  
  unsigned num_stoppers;
};


// ----------------------------- Implementation -----------------------------

template <class Lock>
Reactor<Lock>::Reactor():
  num_stoppers(0)
{
}

template <class Lock>
typename Reactor<Lock>::hs_iterator 
Reactor<Lock>::find_handler_state(Event_handler* eh)
{
  return std::find(begin(), end(), HandlerState(eh->get_handler()));
}

template <class Lock>
iqnet::Event_handler* Reactor<Lock>::find_handler(Socket::Handler fd)
{
  h_iterator i = handlers.find(fd);
  return i == handlers.end() ? 0 : i->second;
}
 
template <class Lock>
void Reactor<Lock>::register_handler( Event_handler* eh, Event_mask mask )
{
  scoped_lock lk(lock);
  
  if (eh->is_stopper())
    num_stoppers++;

  Socket::Handler fd = eh->get_handler();
  
  if( handlers.find(fd) == handlers.end() )
  {
    handlers_states.push_back( HandlerState(fd, mask) );
    handlers[fd] = eh;
  }
  else
  {
    typename Reactor<Lock>::hs_iterator i = find_handler_state(eh);
    i->mask |= mask;
  }
}

template <class Lock>
void Reactor<Lock>::unregister_handler( Event_handler* eh, Event_mask mask )
{
  scoped_lock lk(lock);
  hs_iterator i = find_handler_state( eh );
  
  if( i != end() )
  {
    int newmask = (i->mask &= !mask);

    if( !newmask )
    {
      handlers.erase(eh->get_handler());
      handlers_states.erase(i);

      if (eh->is_stopper())
        num_stoppers--;
    }
  }
}

template <class Lock>
void Reactor<Lock>::unregister_handler( Event_handler* eh )
{
  scoped_lock lk(lock);
  h_iterator i = handlers.find(eh->get_handler());
  
  if( i != handlers.end() )
  {
    handlers.erase(i);
    handlers_states.erase(find_handler_state(eh));

    if (eh->is_stopper())
      num_stoppers--;
  }
}

template <class Lock>
void Reactor<Lock>::fake_event( Event_handler* eh, Event_mask mask )
{
  scoped_lock lk(lock);
  hs_iterator i = find_handler_state( eh );

  if( i != end() )
    i->revents |= mask;
}

template <class Lock>
void Reactor<Lock>::invoke_clients_handler( 
  Event_handler* handler, HandlerState& hs, bool& terminate )
{
  bool in  = hs.revents & Reactor_base::INPUT;
  bool out = hs.revents & Reactor_base::OUTPUT;
  
  if( in )
    handler->handle_input( terminate );
  else if( out )
    handler->handle_output( terminate );
}

template <class Lock>
void Reactor<Lock>::invoke_servers_handler( 
    Event_handler* handler, HandlerState& hs, bool& terminate )
{
  try {
    invoke_clients_handler( handler, hs, terminate );
  }
  catch( const std::exception& e )
  {
    handler->log_exception( e );
    terminate = true;
  }
  catch( ... )
  {
    handler->log_unknown_exception();
    terminate = true;
  }
}

template <class Lock>
void Reactor<Lock>::invoke_event_handler( Reactor<Lock>::HandlerState& hs )
{
  bool terminate = false;

  Event_handler* handler = find_handler(hs.fd);
  assert(handler);
  
  if( handler->catch_in_reactor() )
    invoke_servers_handler( handler, hs, terminate );
  else
    invoke_clients_handler( handler, hs, terminate );
  
  if( terminate )
  {
    unregister_handler( handler );
    handler->finish();
  }
}

template <class Lock>
void Reactor<Lock>::handle_user_events()
{
  HandlerStateList called_by_user;
  scoped_lock lk(lock);
  
  for( hs_iterator i = begin(); i != end(); ++i )
  {
    if( i->revents && (i->mask | i->revents) )
    {
      called_by_user.push_back( *i );
      i->revents &= !i->mask;
    }
  }

  lk.unlock();

  while( !called_by_user.empty() )
  {
    HandlerState hs(called_by_user.front());
    called_by_user.pop_front();
    invoke_event_handler(hs);
  }  
}

template <class Lock>
bool Reactor<Lock>::handle_system_events(Reactor_base::Timeout ms)
{
  scoped_lock lk(lock);
  HandlerStateList tmp(handlers_states);
  lk.unlock();

  // if all events were of "user" type
  if (tmp.empty())
    return true;

  impl.reset(tmp);
  HandlerStateList ret;
  bool succ = impl.poll(ret, ms);

  if (!succ)
    return false;

  while (!ret.empty())
  {
    HandlerState hs(ret.front());
    ret.pop_front();
    invoke_event_handler(hs);
  }

  return true;
}

template <class Lock>
bool Reactor<Lock>::handle_events(Reactor::Timeout ms)
{
  if (handlers.empty())
    return false;

  if (handlers.size() - num_stoppers <= 0)
    throw No_handlers();

  handle_user_events();
  return handle_system_events(ms);
}

} // namespace iqnet

#endif
