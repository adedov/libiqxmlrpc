#include <sstream>
#include <boost/program_options.hpp>
#include <libiqxmlrpc/http_client.h>
#include <libiqxmlrpc/https_client.h>
#include "client_opts.h"

using namespace iqxmlrpc;
using namespace boost::program_options;

Client_opts::Client_opts():
  client_factory_(0),
  port_(0),
  proxy_port_(0),
  use_ssl_(false),
  stop_server_(false),
  opts_()
{
  opts_.add_options()
    ("host", value<std::string>(&host_))
    ("port", value<int>(&port_))
    ("proxy-host", value<std::string>(&proxy_host_))
    ("proxy-port", value<int>(&proxy_port_))
    ("use-ssl", value<bool>(&use_ssl_))
    ("stop-server", value<bool>(&stop_server_));
}

Client_opts::~Client_opts()
{
}

void Client_opts::configure(int argc, char** argv)
{
  variables_map vm;
  store(parse_command_line(argc, argv, opts_), vm);
  notify(vm);

  if (!port_)
    throw_bad_config();
  
  if (use_ssl_)
  {
    namespace ssl = iqnet::ssl;
    if (!ssl::ctx)
      ssl::ctx = ssl::Ctx::client_only();

    client_factory_.reset(new Client_factory<Https_client_connection>());
  }
  else
  {
    client_factory_.reset(new Client_factory<Http_client_connection>());
  }

  client_factory_->set_addr(addr());

  if (proxy_set())
    client_factory_->set_proxy_addr(proxy_addr());
}

void Client_opts::throw_bad_config()
{
  std::ostringstream ss;
  ss << opts_;
  throw Bad_config(ss.str());
}
  
iqnet::Inet_addr Client_opts::get_inet_addr(const std::string& h, int p) const
{
  std::string host = h.empty() ? "localhost" : h;
  return iqnet::Inet_addr(host, p);
}
