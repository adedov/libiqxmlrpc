#include <iostream>
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

private:
  Handlers_box handlers;
  
public:
  unsigned       size()  const { return handlers.size(); }
  const_iterator begin() const { return handlers.begin(); }
  iterator       begin()       { return handlers.begin(); }
  const_iterator end()   const { return handlers.end(); }
  iterator       end()         { return handlers.end(); }

  
  void register_handler( Event_handler*, Event_mask );
  void unregister_handler( Event_handler*, Event_mask );
  void unregister_handler( Event_handler* );  

  void fill_pollfd( struct pollfd* );
  void invoke_event_handler( Handler& );
    
  bool handle_events( Reactor::Timeout );
};
#endif


void Reactor::Reactor_impl::register_handler( Event_handler* eh, Event_mask mask )
{
  int fd = eh->get_fd();
  iterator i = std::find_if( begin(), end(), Handler_fd_eq(fd) );
  
  if( i == end() )
    handlers.push_back( Handler( fd, eh, mask ) );
  else
    i->mask |= mask;
}


void Reactor::Reactor_impl::unregister_handler( Event_handler* eh, Event_mask mask )
{
  int fd = eh->get_fd();
  iterator i = std::find_if( begin(), end(), Handler_fd_eq(fd) );
  
  if( i != end() )
  {
    int newmask = (i->mask &= !mask);
    
    if( !newmask )
      handlers.erase(i);
  }
}


void Reactor::Reactor_impl::unregister_handler( Event_handler* eh )
{
  int fd = eh->get_fd();
  iterator i = std::find_if( begin(), end(), Handler_fd_eq(fd) );
  
  if( i != end() )
    handlers.erase(i);
}


void Reactor::Reactor_impl::fill_pollfd( struct pollfd* pfd )
{
  struct pollfd* piter = pfd;
    
  for( const_iterator i = begin(); i != end(); ++i )
  {
    piter->fd = i->fd;
    piter->events  = i->mask & INPUT ? POLLIN|POLLPRI : 0;
    piter->events |= i->mask & OUTPUT ? POLLOUT : 0;
    piter++;
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

  
bool Reactor::Reactor_impl::handle_events( Reactor::Timeout to_ms )
{
  unsigned hsz = size();
  struct pollfd pfd[hsz];
  
  fill_pollfd( pfd );
  
  int code = poll( pfd, hsz, to_ms );
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


// -----------------------------------------------------------
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


bool Reactor::handle_events( Reactor::Timeout to_ms )
{
  return impl->handle_events( to_ms );
}
