#ifndef _libiqnet_reactor_h_
#define _libiqnet_reactor_h_

namespace iqnet
{
  class Event_handler;
  class Reactor;
};


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

  //! \return true if any handle was invoked, false on timeout.
  bool handle_events( Timeout ms = -1 );
};

#endif
