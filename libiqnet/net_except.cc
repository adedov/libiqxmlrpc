#include <errno.h>
#include <string.h>
#include "net_except.h"


iqnet::network_error::network_error( const std::string& msg, bool use_errno ):
  std::runtime_error( 
    use_errno ? (msg + std::string(": ") + strerror(errno)) : msg )
{
}
