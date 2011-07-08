//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_reactor_poll_impl_h_
#define _iqxmlrpc_reactor_poll_impl_h_

#ifdef HAVE_POLL
#include "reactor.h"

#include <boost/utility.hpp>

namespace iqnet
{

//! Reactor implementation helper based on poll() system call.
class LIBIQXMLRPC_API Reactor_poll_impl: boost::noncopyable {
  struct Impl;
  Impl* impl;

public:
  Reactor_poll_impl();
  virtual ~Reactor_poll_impl();

  void reset(const Reactor_base::HandlerStateList&);
  bool poll(Reactor_base::HandlerStateList& out, Reactor_base::Timeout);
};

} // namespace iqnet

#endif // HAVE_POLL
#endif
