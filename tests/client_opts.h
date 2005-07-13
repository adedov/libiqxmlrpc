#ifndef _libiqxmlrcp_test_suite_client_opts_h_
#define _libiqxmlrcp_test_suite_client_opts_h_

#include <string>
#include <memory>
#include <stdexcept>
#include <boost/program_options.hpp>
#include "libiqxmlrpc/libiqxmlrpc.h"

//! Abstract clients factory base.
//! Its children should create concrete clients 
//! (parametrized with appropriate transport).
class Client_factory_base {
protected:
  iqnet::Inet_addr addr_;

public:
  virtual ~Client_factory_base() {}

  void set_addr(const iqnet::Inet_addr& addr)
  {
    addr_ = addr;
  }
  
  virtual iqxmlrpc::Client_base* create() = 0;
};

//! Template for concrete clients' factory
template <class Transport>
class Client_factory: public Client_factory_base {
public:
  iqxmlrpc::Client_base* create()
  {
    return new iqxmlrpc::Client<Transport>(addr_);
  }
};

//! Generic test clients' configurator
/*! Recognizes following options:
 *    --host
 *    --port
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
  std::auto_ptr<Client_factory_base>   client_factory_;

  std::string host_;
  int         port_;
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
  
  const std::string host()        const { return host_; }
  int               port()        const { return port_; }
  bool              use_ssl()     const { return use_ssl_; }
  bool              stop_server() const { return stop_server_; }

protected:
  void throw_bad_config();
};

#endif
