#ifndef _iqxmlrpc_test_suite_methods_
#define _iqxmlrpc_test_suite_methods_

#include "libiqxmlrpc/libiqxmlrpc.h"

//! Register actual test methods in specified server object.
void register_user_methods(iqxmlrpc::Server& server);

class serverctl_stop: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};

class Echo: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};

class Get_file: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};

#endif
