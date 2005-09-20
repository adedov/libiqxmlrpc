#include <stdlib.h>
#include "server_config.h"

Test_server_config::Malformed_cmd_line::Malformed_cmd_line():
  Malformed_config(
    "Usage:\n\ttest_server <port> <numthreads> [use_ssl]\n")
{
}

Test_server_config::Malformed_xmlrpc_arg::Malformed_xmlrpc_arg():
  Malformed_config(
    "Malformed request. Required fields: port, use-ssl, numthreads")
{
}

Test_server_config::~Test_server_config()
{
}

Test_server_config Test_server_config::create(int argc, char** argv)
{
  if (argc != 3 && argc != 4)
    throw Malformed_cmd_line();

  Test_server_config conf;
  conf.port = atoi(argv[1]);
  conf.numthreads = atoi(argv[2]);

  if (argc == 4)
    conf.use_ssl = true;

  return conf;
}

Test_server_config Test_server_config::create(const iqxmlrpc::Value& v)
{
  Test_server_config conf;

  try {
    conf.port      = v["port"];
    conf.use_ssl   = v["use-ssl"];
    conf.numthreads = int(v["numthreads"]);
  } 
  catch (const iqxmlrpc::Struct::No_field&) 
  {
    throw Malformed_xmlrpc_arg();
  }
  catch (const iqxmlrpc::Value::Bad_cast&) 
  {
    throw Malformed_xmlrpc_arg();
  }

  return conf;
}
