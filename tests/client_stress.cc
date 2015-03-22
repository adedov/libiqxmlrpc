#define BOOST_TEST_MODULE test_client_stress
#include <stdlib.h>
#include <openssl/md5.h>
#include <memory>
#include <iostream>
#include <sstream>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>
#include <boost/timer.hpp>
#include "client_common.h"
#include "client_opts.h"

#if defined(WIN32)
#include <winsock2.h>
#endif

using namespace boost::unit_test;
using namespace boost::program_options;
using namespace iqxmlrpc;

class Stress_test_opts: public Client_opts {
  int client_threads_;
  int calls_per_thread_;

public:
  Stress_test_opts():
    client_threads_(1),
    calls_per_thread_(1)
  {
    opts_.add_options()
      ("client-threads",   value<int>(&client_threads_))
      ("calls-per-thread", value<int>(&calls_per_thread_));
  }

  int client_threads()   const { return client_threads_; }
  int calls_per_thread() const { return calls_per_thread_; }
};

// Global configuration
Stress_test_opts test_config;
Client_base* test_client = 0;

class ClientFixture {
public:
  ClientFixture()
  {
#if defined(WIN32)
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);
#endif

    int argc = boost::unit_test::framework::master_test_suite().argc;
    char** argv = boost::unit_test::framework::master_test_suite().argv;
    test_config.configure(argc, argv);
    test_client = test_config.create_instance();
  }
};

BOOST_GLOBAL_FIXTURE( ClientFixture );

// Stress test thread function
void do_call(Client_base* client)
{
  try {
    Get_file_proxy get_file(client);
    Response r( get_file(65536) );

    if (r.is_fault())
      std::cerr << "Fault response: " << r.fault_string() << std::endl;
  }
  catch(const std::exception& e)
  {
    std::cerr << "E: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unexpected exception" << std::endl;
  }
}

void do_test()
{
  try {
    std::auto_ptr<Client_base> client(test_config.create_instance());
    for (int i = 0; i < test_config.calls_per_thread(); ++i) {
      do_call(client.get());
    }
  }
  catch(const std::exception& e)
  {
    std::cerr << "E: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unexpected exception" << std::endl;
  }
}

BOOST_AUTO_TEST_CASE( stress_test )
{
  boost::timer timer;
  boost::thread_group thrds;

  for(int i = 0; i < test_config.client_threads(); ++i)
    thrds.create_thread(&do_test);

  thrds.join_all();

  std::ostringstream ss;
  ss << "Stress test elapsed time: " << timer.elapsed();
  BOOST_MESSAGE(ss.str());
}

// vim:ts=2:sw=2:et
