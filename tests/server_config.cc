#include <stdlib.h>
#include "server_config.h"

Test_server_config::Malformed_cmd_line::Malformed_cmd_line():
  Malformed_config(
    "Usage:\n\ttest_server <port> <numthreads> [use_ssl]\n")
{
}

Test_server_config::Test_server_config(int argc, const char** argv)
{
  if (argc != 3 && argc != 4)
    throw Malformed_cmd_line();

  port = atoi(argv[1]);
  numthreads = atoi(argv[2]);

  if (argc == 4)
    use_ssl = true;
}
