//  Libiqxmlrpc - an object-oriented XML-RPC solution.
//  Copyright (C) 2011 Anton Dedov

#ifndef _libiqnet_lock_h_
#define _libiqnet_lock_h_

#include "api_export.h"

namespace iqnet
{

//! Class which provides null synchronization.
class LIBIQXMLRPC_API Null_lock {
public:
  struct scoped_lock {
    scoped_lock(Null_lock&) {}
    ~scoped_lock() {}

    void lock() {}
    void unlock() {}
  };

  void lock() {}
  void unlock() {}
};

} // namespace iqnet

#endif
