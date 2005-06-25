#include <sstream>
#include <boost/program_options.hpp>
#include <libiqxmlrpc/http_client.h>
#include <libiqxmlrpc/https_client.h>
#include "client_opts.h"

using namespace iqxmlrpc;
using namespace boost::program_options;

Client_opts::Client_opts():
  main_client_(0),
  client_factory_(0),
  port_(0),
  use_ssl_(false),
  num_threads_(1),
  opts_()
{
  opts_.add_options()
    ("host", value<std::string>(&host_))
    ("port", value<int>(&port_))
    ("use-ssl",    value<bool>(&use_ssl_))
    ("numthreads", value<int>(&num_threads_));
}

Client_opts::~Client_opts()
{
}

void Client_opts::configure(int argc, char** argv)
{
  variables_map vm;
  store(parse_command_line(argc, argv, opts_), vm);
  notify(vm);

  if (host_.empty() || !port_)
    throw_bad_config();
  
  iqnet::Inet_addr addr(host_, port_);
  main_client_.reset(new Client<Http_client_connection>(addr));

  if (use_ssl_)
    client_factory_.reset(new Client_factory<Https_client_connection>());
  else
    client_factory_.reset(new Client_factory<Http_client_connection>());
}

void Client_opts::throw_bad_config()
{
  std::ostringstream ss;
  ss << opts_;
  throw Bad_config(ss.str());
}
