#include <signal.h>
#include <iostream>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/http_server.h"
#include "libiqxmlrpc/https_server.h"
#include "server_config.h"
#include "methods.h"

using namespace boost::unit_test_framework;

class Test_server {
  Test_server_config conf_;
  iqxmlrpc::Server impl_;

public:
  Test_server(const Test_server_config&);

  iqxmlrpc::Server& impl() { return impl_; }

  void work();
};

Test_server::Test_server(const Test_server_config& conf):
  conf_(conf),
  impl_(conf_.port, conf_.exec_factory)
{
  impl_.log_errors( &std::cerr );
  impl_.enable_introspection();
  register_user_methods(impl());

  if (conf.use_ssl && !iqnet::ssl::ctx)
  {
    namespace ssl = iqnet::ssl;
    ssl::ctx = ssl::Ctx::server_only("data/cert.pem", "data/pk.pem");
  }
}

void Test_server::work()
{
  if (conf_.use_ssl)
    impl_.work<iqxmlrpc::Https_server_connection>();
  else
    impl_.work<iqxmlrpc::Http_server_connection>();
}

Test_server* test_server = 0;

// Ctrl-C handler
void test_server_sig_handler(int)
{
  if (test_server)
    test_server->impl().set_exit_flag();
}

// Tests.
void start_test_server()
{
  BOOST_REQUIRE(test_server);
  test_server->work();
}

test_suite* init_unit_test_suite(int argc, char* argv[])
{
  try {
    Test_server_config conf = Test_server_config::create(argc, argv);
    test_server = new Test_server(conf);
    ::signal(SIGINT, &test_server_sig_handler);
  
    test_suite* test = BOOST_TEST_SUITE("Server test");
    test->add( BOOST_TEST_CASE(&start_test_server) );
  
    return test;
  }
  catch(const iqxmlrpc::Exception& e)
  {
    std::cerr << "iqxmlrpc E: " << e.what() << std::endl;
    throw;
  }
  catch(const iqnet::network_error& e)
  {
    std::cerr << "iqnet E: " << e.what() << std::endl;
    throw;
  }
  catch(const Test_server_config::Malformed_config& e)
  {
    std::cerr << e.what() << std::endl;
    return 0;
  }
  catch(...)
  {
    std::cerr << "Unexpected exception" << std::endl;
    throw;
  }
}
