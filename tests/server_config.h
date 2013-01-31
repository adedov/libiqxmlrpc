#ifndef _iqxmlrpc_test_server_config_h_
#define _iqxmlrpc_test_server_config_h_

#include <stdexcept>
#include "libiqxmlrpc/value.h"

//! Test server configuration structure
struct Test_server_config {
  class Malformed_config;
  class Malformed_cmd_line;

  bool use_ssl;
  int port;
  unsigned numthreads;

  Test_server_config(int argc, const char** argv);
};

class Test_server_config::Malformed_config: public std::runtime_error {
public:
  Malformed_config(const std::string& usage):
    runtime_error(usage) {}
};

class Test_server_config::Malformed_cmd_line:
  public Test_server_config::Malformed_config
{
public:
  Malformed_cmd_line();
};

#endif
