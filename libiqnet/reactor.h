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
//  $Id: reactor.h,v 1.5 2004-03-29 06:20:15 adedov Exp $

#ifndef _libiqnet_reactor_h_
#define _libiqnet_reactor_h_

namespace iqnet
{
  class Event_handler;
  class Reactor;
};


//! Base class for event-driven communication classes.
class iqnet::Event_handler {
public:
  virtual ~Event_handler() {}

  virtual void handle_input( bool& terminate ) {}
  virtual void handle_output( bool& terminate ) {}

  virtual void handle_io( bool& terminate )
  {
    handle_input( terminate );
    
    if( !terminate )
      handle_output( terminate );
  }
  
  //! Invoked by Reactor when handle_X() 
  //! sets terminate variable to true.
  virtual void finish() {};
  
  virtual int get_fd() const = 0;
};


//! Reactor class. 
/*! Invokes appropriate Event_handler's methods when the event, 
    for which they were registered, has been happen.
*/
class iqnet::Reactor {
public:
  enum Event_mask { INPUT=1, OUTPUT=2 };
  typedef int Timeout;

private:
  class Reactor_impl;
  Reactor_impl* impl;
  
public:
  Reactor();
  virtual ~Reactor();

  void register_handler( Event_handler*, Event_mask );
  void unregister_handler( Event_handler*, Event_mask );
  void unregister_handler( Event_handler* );

  void fake_event( Event_handler*, Event_mask );

  //! \return true if any handle was invoked, false on timeout.
  bool handle_events( Timeout ms = -1 );
};

#endif
