#include <iostream>
#include <vector>
#include <list>
#include <deque>
#include <functional>
#include <sys/poll.h>
#include <libiqnet/reactor.h>
#include <libiqnet/net_except.h>

using namespace iqnet;

#ifndef DOXYGEN_SHOULD_SKIP_THIS
class Reactor::Reactor_impl {
public:
  struct Handler {
    int            fd;
    Event_handler* handler;
    short          mask;
    short          revents;
    
    Handler():
      fd(0), handler(0), mask(0), revents(0) {}
    
    Handler( int fd_, Event_handler* eh, Reactor::Event_mask m ):
      fd(fd_), handler(eh), mask(m), revents(0) {}
  };
  
  class Handler_fd_eq: public std::unary_function<bool, Handler> {
    int fd;
  public:
    Handler_fd_eq( int fd_ ): fd(fd_) {}
  
    bool operator ()( const Handler& h )
    {
      return fd == h.fd;
    }
  };

  typedef std::list<Handler> Handlers_box;
  typedef Handlers_box::const_iterator const_iterator;
  typedef Handlers_box::iterator iterator;
  typedef std::vector<struct pollfd> Pollfd_vec;

private:
  Handlers_box handlers;
  Handlers_box called_by_user;
  Pollfd_vec   pfd;
  
public:
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
  void handle_user_events();
  bool handle_system_events( Reactor::Timeout );
};
#endif


inline Reactor::Reactor_impl::iterator 
  Reactor::Reactor_impl::find_handler( Event_handler* eh )
{
  int fd = eh->get_fd();
  return std::find_if( begin(), end(), Handler_fd_eq(fd) );
}


void Reactor::Reactor_impl::register_handler( Event_handler* eh, Event_mask mask )
{
  iterator i = find_handler( eh );
  
  if( i == end() )
    handlers.push_back( Handler( eh->get_fd(), eh, mask ) );
  else
    i->mask |= mask;
}


void Reactor::Reactor_impl::unregister_handler( Event_handler* eh, Event_mask mask )
{
  iterator i = find_handler( eh );
  
  if( i != end() )
  {
    int newmask = (i->mask &= !mask);

    if( !newmask )
      handlers.erase(i);
  }
}


void Reactor::Reactor_impl::unregister_handler( Event_handler* eh )
{
  iterator i = find_handler( eh );
  
  if( i != end() )
    handlers.erase(i);
}


void Reactor::Reactor_impl::fake_event( Event_handler* eh, Event_mask mask )
{
  iterator i = find_handler( eh );

  if( i == end() )
    return;
  
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
      i->revents &= !i->mask;
    }
  }
}


void Reactor::Reactor_impl::prepare_system_events()
{
  pfd.clear();
    
  for( const_iterator i = begin(); i != end(); ++i )
  {
    short events = i->mask & INPUT ? POLLIN|POLLPRI : 0;
    events |= i->mask & OUTPUT ? POLLOUT : 0;
    struct pollfd sfd = { i->fd, events, 0 };
    pfd.push_back( sfd );
  }
}


void Reactor::Reactor_impl::invoke_event_handler( Handler& h )
{
  bool in  = h.revents & POLLIN;
  bool out = h.revents & POLLOUT || h.revents & POLLPRI;
  bool err = h.revents & POLLHUP || h.revents & POLLERR || h.revents & POLLNVAL;
  bool terminate = false;

  if( in && out )
    h.handler->handle_io( terminate );
  else if( in )
    h.handler->handle_input( terminate );
  else if( out )
    h.handler->handle_output( terminate );
  else if( err )
    terminate = true;
  
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
  prepare_system_events();
  unsigned hsz = size();
  
  int code = poll( &pfd[0], hsz, to_ms );
  if( code < 0 )
    throw network_error( "poll" );
  
  if( !code )
    return false;
  
  std::deque<Handler> active_hs;
  for( int i = 0; i < hsz; i++ )
  {
    if( pfd[i].revents )
    {
      const_iterator j = std::find_if( begin(), end(), Handler_fd_eq(pfd[i].fd) );
      Handler h( *j );
      h.revents = pfd[i].revents;
      active_hs.push_back( h );
    }
  }
  
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
Reactor::Reactor():
  impl(new Reactor_impl)
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
  return impl->handle_events( to_ms );
}
