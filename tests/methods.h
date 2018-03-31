#ifndef _iqxmlrpc_test_suite_methods_
#define _iqxmlrpc_test_suite_methods_

#include "libiqxmlrpc/libiqxmlrpc.h"

//! Register actual test methods in specified server object.
void register_user_methods(iqxmlrpc::Server& server);

class serverctl_stop: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};

void echo_method(iqxmlrpc::Method*, const iqxmlrpc::Param_list&, iqxmlrpc::Value&);
void echo_user(iqxmlrpc::Method*, const iqxmlrpc::Param_list&, iqxmlrpc::Value&);
void trace_method(iqxmlrpc::Method*, const iqxmlrpc::Param_list&, iqxmlrpc::Value&);
void error_method(iqxmlrpc::Method*, const iqxmlrpc::Param_list&, iqxmlrpc::Value&);

class Get_file: public iqxmlrpc::Method {
public:
  void execute( const iqxmlrpc::Param_list&, iqxmlrpc::Value& );
};

#endif
