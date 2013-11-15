#ifndef _libiqxmlrcp_test_suite_client_opts_h_
#define _libiqxmlrcp_test_suite_client_opts_h_

#include <string>
#include <stdexcept>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/client.h"

//! Generic test clients' configurator
/*! Recognizes following options:
 *    --host
 *    --port
 *    --proxy-host
 *    --proxy-port
 *    --use-ssl
 *    --numthreads
 *    --timeout
 */
class Client_opts {
public:
  class Bad_config: public std::runtime_error {
  public:
    Bad_config(const std::string& usage):
      runtime_error(usage) {}
  };

private:
  std::string host_;
  int         port_;
  std::string proxy_host_;
  int         proxy_port_;
  bool        use_ssl_;
  bool        stop_server_;
  int         timeout_;
  std::string server_fingerprint_;

protected:
  boost::program_options::options_description opts_;

public:
  Client_opts();
  virtual ~Client_opts();

  //! Process actual configuration from cmd line parameters.
  //! Run it before any other methods of this class!
  virtual void configure(int argc, char** argv);

  iqxmlrpc::Client_base* create_instance() const;

  iqnet::Inet_addr  addr()        const { return get_inet_addr(host_, port_); }
  iqnet::Inet_addr  proxy_addr()  const { return get_inet_addr(proxy_host_, proxy_port_); }
  bool              proxy_set()   const { return proxy_port_; }
  bool              use_ssl()     const { return use_ssl_; }
  bool              stop_server() const { return stop_server_; }
  int               timeout()     const { return timeout_; }

protected:
  void throw_bad_config();

private:
  iqnet::Inet_addr  get_inet_addr(const std::string&, int) const;
};

#endif
