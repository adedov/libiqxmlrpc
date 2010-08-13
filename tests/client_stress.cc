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

// Stress test thread function
void do_test()
{
  BOOST_MESSAGE("Threaded_client started.");

  try {
    std::auto_ptr<Client_base> client(test_config.create_instance());
    Get_file_proxy get_file(client.get());

    for (int i = 0; i < test_config.calls_per_thread(); ++i) {
      Response r( get_file(65536) );
      BOOST_REQUIRE(!r.is_fault());
    }
  }
  catch(const std::exception& e)
  {
    BOOST_ERROR(e.what());
  }
  catch(...)
  {
    BOOST_ERROR("Unexpected exception");
  }

  BOOST_MESSAGE("Threaded_client stoped.");
}

void stop_test_server()
{
  std::auto_ptr<Client_base> client(test_config.create_instance());
  Stop_server_proxy stop(client.get());
  Response r( stop() );
  BOOST_REQUIRE(!r.is_fault());
}

void stress_test()
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

bool init_tests()
{
  test_suite& test = framework::master_test_suite();
  test.add( BOOST_TEST_CASE(&stress_test) );

  if (test_config.stop_server())
    test.add( BOOST_TEST_CASE(&stop_test_server) );

  return true;
}

int main(int argc, char* argv[])
{
  try {
    test_config.configure(argc, argv);
    boost::unit_test::unit_test_main( &init_tests, argc, argv );
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}

// vim:ts=2:sw=2:et
