//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_reactor_select_impl_h_
#define _iqxmlrpc_reactor_select_impl_h_

#ifndef HAVE_POLL
#include "reactor.h"

#include <boost/utility.hpp>

namespace iqnet
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#endif

//! Reactor implementation helper based on select() system call.
class LIBIQXMLRPC_API Reactor_select_impl: boost::noncopyable {
  Socket::Handler max_fd;
  fd_set read_set, write_set, err_set;
  Reactor_base::HandlerStateList hs;

public:
  Reactor_select_impl();
  virtual ~Reactor_select_impl();

  void reset(const Reactor_base::HandlerStateList&);
  bool poll(Reactor_base::HandlerStateList& out, Reactor_base::Timeout);
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqnet

#endif // not defined HAVE_POLL
#endif
