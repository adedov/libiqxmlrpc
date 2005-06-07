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

//! An implementation class for "serverctl.start"
class serverctl_start: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};

//! Base class for all XML-RPC servers in this test suite.
/*! Supports "serverctl.stop" as basic functionality.
 */
class Test_server_base {
  Test_server_config conf_;
  iqxmlrpc::Server impl_;

public:
  Test_server_base(const Test_server_config&);

  iqxmlrpc::Server& impl() { return impl_; }

  void work();
};

//! Test servers manager.
/*! Starts User_test_server's instances from "serverctl.start" method.
 */
class Test_suite_server: public Test_server_base {
public:
  Test_suite_server(const Test_server_config&);
};

//! User's test server.
/*! It is created by server manager.
    Implements all needed test functionality.
 */
class User_test_server: public Test_server_base {
public:
  User_test_server(const Test_server_config&);
};

#endif
