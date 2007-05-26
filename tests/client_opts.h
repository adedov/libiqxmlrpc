#ifndef _libiqxmlrcp_test_suite_client_opts_h_
#define _libiqxmlrcp_test_suite_client_opts_h_

#include <string>
#include <stdexcept>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/client.h"

//! Abstract clients factory base.
//! Its children should create concrete clients 
//! (parametrized with appropriate transport).
class Client_factory_base {
protected:
  iqnet::Inet_addr addr_;
  boost::optional<iqnet::Inet_addr> proxy_addr_;

public:
  virtual ~Client_factory_base() {}

  void set_addr(const iqnet::Inet_addr& addr)
  {
    addr_ = addr;
  }
  
  void set_proxy_addr(const iqnet::Inet_addr& addr)
  {
    proxy_addr_ = addr;
  }

  virtual iqxmlrpc::Client_base* create() = 0;
};

//! Template for concrete clients' factory
template <class Transport>
class Client_factory: public Client_factory_base {
public:
  iqxmlrpc::Client_base* create()
  {
    iqxmlrpc::Client<Transport>* client =
      new iqxmlrpc::Client<Transport>(addr_);

    if (proxy_addr_)
      client->set_proxy(proxy_addr_.get());

    return client;
  }
};

//! Generic test clients' configurator
/*! Recognizes following options:
 *    --host
 *    --port
 *    --proxy-host
 *    --proxy-port
 *    --use-ssl
 *    --numthreads
 */
class Client_opts {
public:
  class Bad_config: public std::runtime_error {
  public:
    Bad_config(const std::string& usage):
      runtime_error(usage) {}
  };

private:
  boost::scoped_ptr<Client_factory_base> client_factory_;

  std::string host_;
  int         port_;
  std::string proxy_host_;
  int         proxy_port_;
  bool        use_ssl_;
  bool        stop_server_;

protected:
  boost::program_options::options_description opts_;

public:
  Client_opts();
  virtual ~Client_opts();

  //! Process actual configuration from cmd line parameters.
  //! Run it before any other methods of this class!
  virtual void configure(int argc, char** argv);

  //! Returns clients factory that can create test server clients
  //! according to current configuration
  Client_factory_base* client_factory() const { return client_factory_.get(); }
  
  iqnet::Inet_addr  addr()        const { return get_inet_addr(host_, port_); }
  iqnet::Inet_addr  proxy_addr()  const { return get_inet_addr(proxy_host_, proxy_port_); }
  bool              proxy_set()   const { return proxy_port_; }
  bool              use_ssl()     const { return use_ssl_; }
  bool              stop_server() const { return stop_server_; }

protected:
  void throw_bad_config();

private:
  iqnet::Inet_addr  get_inet_addr(const std::string&, int) const;
};

#endif
