#include <errno.h>
#include <string.h>
#include "net_except.h"


iqnet::network_error::network_error( const std::string& where ):
  std::runtime_error( where + std::string(": ") + strerror(errno) )
{
}
