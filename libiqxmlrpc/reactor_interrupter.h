//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _iqxmlrpc_reactor_interrupter_h_
#define _iqxmlrpc_reactor_interrupter_h_

#include "reactor.h"

namespace iqnet {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4275)
#endif

class LIBIQXMLRPC_API Reactor_interrupter {
public:
  Reactor_interrupter(Reactor_base*);
  Reactor_interrupter(const Reactor_interrupter&) = delete;
  Reactor_interrupter(Reactor_interrupter&&) = delete;
  Reactor_interrupter& operator=(const Reactor_interrupter&) = delete;
  Reactor_interrupter& operator=(Reactor_interrupter&&) = delete;
  ~Reactor_interrupter();

  void make_interrupt();

private:
  class Impl;
  Impl* impl_;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace iqnet

#endif
