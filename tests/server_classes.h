#ifndef _iqxmlrpc_test_suite_server_base_
#define _iqxmlrpc_test_suite_server_base_

#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/server.h"
#include "server_config.h"

namespace iqxmlrpc 
{
  class Executor_factory_base;
}

//! An implementation class for "serverctl.stop"
class serverctl_stop: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};

class Test_server {
  Test_server_config conf_;
  iqxmlrpc::Server impl_;

public:
  Test_server(const Test_server_config&);

  iqxmlrpc::Server& impl() { return impl_; }

  void work();
};

#endif
