/*! The framework for testing XML-RPC server side.
    This application runs simple single-threaded
    XML-RPC server that supports only two methods:

      * serverctl.start
          Creates new thread of execution and starts
          an XML-RPC server within it. This server
          supports all user defined "test" methods.

      * serverctl.stop
          Stops the application. This method also
          is supported by every server started by
          serverctl.start.
*/

#include <signal.h>
#include <iostream>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "server_classes.h"

using namespace boost::unit_test_framework;

Test_suite_server* test_server = 0;

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
    test_server = new Test_suite_server(conf);
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
  }
}
