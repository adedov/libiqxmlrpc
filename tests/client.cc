#include <stdlib.h>
#include <iostream>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/http_client.h"
//#include "client.h"

using namespace boost::unit_test_framework;
using namespace iqxmlrpc;

typedef Client<Http_client_connection> Server_manager_client;
typedef Client<Http_client_connection> Http_client;

//! Data related with connection with Test Suite Management Server
namespace TestMgr 
{
  std::string host;
  int port = 0;
  Server_manager_client* client = 0;
}

//! HTTP test server's client.
Http_client* http_client = 0;

void stop_main_server()
{
  BOOST_REQUIRE(TestMgr::client);
  Response retval = TestMgr::client->execute("serverctl.stop", Nil());
  BOOST_CHECK(!retval.is_fault());
}

void start_http_server()
{
  BOOST_REQUIRE(TestMgr::client);

  Struct param;
  param.insert("port", TestMgr::port+1);
  param.insert("use-ssl", false);
  param.insert("numthreads", 1);

  Response retval( TestMgr::client->execute("serverctl.start", Value(param)) );
  BOOST_REQUIRE(!retval.is_fault());

  iqnet::Inet_addr addr(TestMgr::host, TestMgr::port+1);
  http_client = new Http_client(addr);
  BOOST_CHECK(http_client);
}

void stop_http_server()
{
  BOOST_REQUIRE(http_client);
  Response retval = http_client->execute("serverctl.stop", Nil());
  BOOST_REQUIRE(!retval.is_fault());
}

test_suite* init_unit_test_suite(int argc, char* argv[])
{
  std::string prg_name(argv[0]);
  unsigned i = prg_name.find_last_of("/\\");
  if (i != std::string::npos)
    prg_name.erase(0, i+1);

  if (argc != 3)
  {
    std::cerr << "Usage:\n\t" << prg_name << " <host> <port>" << std::endl;
    return 0;
  }

  TestMgr::host = argv[1];
  TestMgr::port = atoi(argv[2]);
  iqnet::Inet_addr addr(argv[1], TestMgr::port);
  TestMgr::client = new Server_manager_client(addr);

  test_suite* test = 0;
  
  if (prg_name.find("stop-test-server") != std::string::npos)
  {
    test = BOOST_TEST_SUITE("Stop management server test");
    test->add( BOOST_TEST_CASE(&stop_main_server) );
  }
  else
  {
    test = BOOST_TEST_SUITE("Client test");
    test->add( BOOST_TEST_CASE(&start_http_server) );
    test->add( BOOST_TEST_CASE(&stop_http_server) );
//  test->add( BOOST_TEST_CASE(&test_failed_call) );
//  test->add( BOOST_TEST_CASE(&test_get_file) );
//  test->add( BOOST_TEST_CASE(&stop_server) );
  }

  return test;
}
