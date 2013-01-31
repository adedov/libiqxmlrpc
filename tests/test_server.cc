#define BOOST_TEST_MODULE test_server
#include <signal.h>
#include <memory>
#include <iostream>
#include <boost/utility.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/http_server.h"
#include "libiqxmlrpc/https_server.h"
#include "libiqxmlrpc/executor.h"
#include "libiqxmlrpc/auth_plugin.h"
#include "server_config.h"
#include "methods.h"

using namespace boost::unit_test;
using namespace iqxmlrpc;

class LogInterceptor: public Interceptor {
public:
  void process(Method* m, const Param_list& p, Value& r)
  {
    std::cout << "Log Interceptor: " << m->name() << " is executing.\n";
    yield(m, p, r);
  }
};

class CallCountingInterceptor: public Interceptor {
  unsigned count;

public:
  CallCountingInterceptor(): count(0) {}

  ~CallCountingInterceptor()
  {
    std::cout << "Calls Count: " << count << std::endl;
  }

  void process(Method* m, const Param_list& p, Value& r)
  {
    std::cout << "Executing " << ++count << " call\n";
    yield(m, p, r);
  }
};

class PermissiveAuthPlugin: public iqxmlrpc::Auth_Plugin_base {
public:
  PermissiveAuthPlugin() {}

  bool do_authenticate(const std::string& username, const std::string& pw) const
  {
    return username != "badman";
  }

  bool do_authenticate_anonymous() const
  {
    return true;
  }
};

class Test_server: boost::noncopyable {
  std::auto_ptr<Executor_factory_base> ef_;
  std::auto_ptr<Server> impl_;
  PermissiveAuthPlugin auth_plugin_;

public:
  Test_server(const Test_server_config&);

  Server& impl() { return *impl_.get(); }

  void work();
};

Test_server* test_server = 0;

Test_server::Test_server(const Test_server_config& conf):
  ef_(0),
  impl_(0)
{
  if (conf.numthreads > 1)
  {
    ef_.reset(new Pool_executor_factory(conf.numthreads));
  }
  else
  {
    ef_.reset(new Serial_executor_factory);
  }

  if (conf.use_ssl)
  {
    namespace ssl = iqnet::ssl;
    ssl::ctx = ssl::Ctx::server_only("data/cert.pem", "data/pk.pem");
    impl_.reset(new Https_server(conf.port, ef_.get()));
  }
  else
  {
    impl_.reset(new Http_server(conf.port, ef_.get()));
  }

  impl_->push_interceptor(new CallCountingInterceptor);
  impl_->push_interceptor(new LogInterceptor);

  impl_->log_errors( &std::cerr );
  impl_->enable_introspection();
  impl_->set_max_request_sz(1024*1024);
  impl_->set_verification_level(http::HTTP_CHECK_STRICT);

  impl_->set_auth_plugin(auth_plugin_);

  register_user_methods(impl());
}

void Test_server::work()
{
  impl_->work();
}

// Ctrl-C handler
void test_server_sig_handler(int)
{
  if (test_server)
    test_server->impl().set_exit_flag();
}

class TestConfig {
public:
  TestConfig()
  {
      ::signal(SIGINT, &test_server_sig_handler);
  }
};

BOOST_GLOBAL_FIXTURE( TestConfig );

BOOST_AUTO_TEST_CASE( start_test_server )
{
  Test_server_config conf;
  test_server = new Test_server(conf);
  test_server->work();
}

// vim:ts=2:sw=2:et
