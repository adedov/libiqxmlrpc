#ifndef _libiqnet_net_except_h_
#define _libiqnet_net_except_h_

#include <stdexcept>
#include <errno.h>

namespace iqnet 
{
  class network_error;
};


//! Exception class to wrap a network's subsystem errors.
class iqnet::network_error: public std::runtime_error {
public:
  network_error( const std::string& where ):
    std::runtime_error( where + std::string(": ") + strerror(errno) ) {}
};

#endif
