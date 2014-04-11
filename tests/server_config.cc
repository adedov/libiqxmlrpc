#include <iostream>
#include <sstream>
#include <boost/program_options.hpp>
#include "server_config.h"
#include "libiqxmlrpc/value.h"

using namespace boost::program_options;

void throw_bad_config(options_description& opts)
{
  std::ostringstream ss;
  ss << opts;
  throw Test_server_config::Bad_config(ss.str());
}

Test_server_config::Test_server_config(int argc, char** argv):
  port(0),
  numthreads(1),
  use_ssl(false),
  omit_string_tags(false)
{
  options_description opts;
  opts.add_options()
    ("port", value<int>(&port))
    ("numthreads", value<int>(&numthreads))
    ("use-ssl", value<bool>(&use_ssl))
    ("omit-string-tags", value<bool>(&omit_string_tags));

  variables_map vm;
  store(parse_command_line(argc, argv, opts), vm);
  notify(vm);

  if (!port)
    throw_bad_config(opts);

  if (omit_string_tags)
  {
    std::cout << "Omit string tags in responses" << std::endl;
    iqxmlrpc::Value::omit_string_tag_in_responses(true);
  }
}
