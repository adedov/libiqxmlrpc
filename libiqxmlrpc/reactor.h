//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_reactor_h_
#define _libiqnet_reactor_h_

#include "lock.h"
#include "net_except.h"
#include "socket.h"

#include <list>

namespace iqnet
{

//! Base class for event-driven communication classes.
class LIBIQXMLRPC_API Event_handler {
public:
  virtual ~Event_handler() {}

  //! If this handler used as Reactor stopper.
  virtual bool is_stopper() const { return false; }

  virtual void handle_input( bool& /* terminate */) {}
  virtual void handle_output( bool& /* terminate */) {}

  //! Invoked by Reactor when handle_X()
  //! sets terminate variable to true.
  virtual void finish() {};

  //! Whether reactor should catch its exceptions.
  virtual bool catch_in_reactor() const { return false; }
  //! Log its exception catched in an external object.
  virtual void log_exception( const std::exception& ) {};
  //! Log its exception catched in an external object.
  virtual void log_unknown_exception() {};

  virtual Socket::Handler get_handler() const = 0;
};

//! Abstract base for Reactor template.
//! It defines interface, standard exceptions and
//! general data structures for all implementations.
class LIBIQXMLRPC_API Reactor_base {
public:
  class No_handlers: public iqnet::network_error {
  public:
    No_handlers():
      network_error( "iqnet::Reactor: no handlers given.", false ) {}
  };

  enum Event_mask { INPUT=1, OUTPUT=2 };

  struct HandlerState {
    Socket::Handler fd;
    short           mask;
    short           revents;

    HandlerState( Socket::Handler fd_ = 0 ):
      fd(fd_), mask(0), revents(0) {}

    HandlerState( Socket::Handler fd_, Event_mask m ):
      fd(fd_), mask(m), revents(0) {}

    bool operator ==(const HandlerState& hs)
    {
      return fd == hs.fd;
    }
  };

  typedef std::list<HandlerState> HandlerStateList;
  typedef int Timeout;

  virtual ~Reactor_base() {};

  virtual void register_handler( Event_handler*, Event_mask )   = 0;
  virtual void unregister_handler( Event_handler*, Event_mask ) = 0;
  virtual void unregister_handler( Event_handler* ) = 0;
  virtual void fake_event( Event_handler*, Event_mask ) = 0;

  //! \return true if any handle was invoked, false on timeout.
  /*! Throws Reactor::No_handlers when no one handler has been registered. */
  virtual bool handle_events( Timeout ms = -1 ) = 0;
};

} // namespace iqnet

#endif
