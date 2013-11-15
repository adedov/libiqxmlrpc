#include <sstream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <libiqxmlrpc/http_client.h>
#include <libiqxmlrpc/https_client.h>
#include "client_opts.h"

using namespace iqxmlrpc;
namespace ssl = iqnet::ssl;
using namespace boost::program_options;

Client_opts::Client_opts():
  port_(0),
  proxy_port_(0),
  use_ssl_(false),
  stop_server_(false),
  timeout_(0),
  opts_()
{
  opts_.add_options()
    ("host", value<std::string>(&host_))
    ("port", value<int>(&port_))
    ("proxy-host", value<std::string>(&proxy_host_))
    ("proxy-port", value<int>(&proxy_port_))
    ("use-ssl", value<bool>(&use_ssl_))
    ("stop-server", value<bool>(&stop_server_))
    ("timeout", value<int>(&timeout_))
    ("server-finger", value<std::string>(&server_fingerprint_));
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
}

class FingerprintVerifier: public ssl::ConnectionVerifier {
public:
  FingerprintVerifier(const std::string& finger):
    finger_(finger)
  {
    boost::erase_all(finger_, ":");
    boost::to_lower(finger_);
  }

private:
  int do_verify(bool, X509_STORE_CTX* ctx) const
  {
    return finger_ == cert_finger_sha256(ctx);
  }

  mutable std::string finger_;
};

boost::optional<FingerprintVerifier> server_verifier;

iqxmlrpc::Client_base*
Client_opts::create_instance() const
{
  Client_base* retval = 0;

  if (use_ssl_)
  {
    if (!ssl::ctx)
      ssl::ctx = ssl::Ctx::client_only();

    retval = new Client<Https_client_connection>(addr());
  }
  else
  {
    retval = new Client<Http_client_connection>(addr());
  }

  if (proxy_set())
    retval->set_proxy(proxy_addr());

  if (timeout())
    retval->set_timeout(timeout());

  if (use_ssl_ && server_fingerprint_.size()) {
    server_verifier = FingerprintVerifier(server_fingerprint_);
    ssl::ctx->verify_server(&server_verifier.get());
  }

  return retval;
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
