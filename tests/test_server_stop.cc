#include <memory>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/http_server.h"
#include "libiqxmlrpc/executor.h"

#if BOOST_VERSION >= 105000
#define MY_BOOST_TIME_UTC boost::TIME_UTC_
#else
#define MY_BOOST_TIME_UTC boost::TIME_UTC
#endif

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

using namespace boost::unit_test_framework;
using namespace iqxmlrpc;

std::chrono::microseconds from_xtime(const boost::xtime& xt) {
    return std::chrono::seconds(xt.sec) + std::chrono::microseconds(xt.usec);
}

class TestServer {
public:
  TestServer(int port, unsigned threads):
    exec_factory_(threads > 1 ?
      static_cast<Executor_factory_base*>(new Pool_executor_factory(threads)) :
      static_cast<Executor_factory_base*>(new Serial_executor_factory)),
    serv_(new Http_server(port, exec_factory_.get())),
    thread_(new std::thread(std::bind(&TestServer::run, this)))
  {
  }
  TestServer(const TestServer&) = delete;
  TestServer(TestServer&&) = delete;
  TestServer& operator=(const TestServer&) = delete;
  TestServer& operator=(TestServer&&) = delete;

  void stop()
  {
    serv_->set_exit_flag();
  }

  void join()
  {
    thread_->join();
  }

private:
  std::unique_ptr<iqxmlrpc::Executor_factory_base> exec_factory_;
  std::unique_ptr<iqxmlrpc::Server> serv_;
  std::unique_ptr<std::thread> thread_;

  static void run(TestServer* obj)
  {
    obj->serv_->work();
  }
};

void stop_and_join(unsigned threads, std::condition_variable* on_stop)
{
  TestServer s(3344, threads);

  boost::xtime time_wait;
  boost::xtime_get(&time_wait, MY_BOOST_TIME_UTC);
  time_wait.sec += 1;

  std::this_thread::sleep_for(from_xtime(time_wait));

  s.stop();
  s.join();
  on_stop->notify_all();
}

void stop_test_server(unsigned server_threads)
{
  std::condition_variable stopped;
  std::mutex c_mutex;

  std::thread thr(std::bind(stop_and_join, server_threads, &stopped));

  {
  boost::xtime time_wait;
  boost::xtime_get(&time_wait, MY_BOOST_TIME_UTC);
  time_wait.sec += 1;
  std::this_thread::sleep_for(from_xtime(time_wait));
  }

  boost::xtime time_wait;
  boost::xtime_get(&time_wait, MY_BOOST_TIME_UTC);
  time_wait.sec += 3;

  std::lock_guard lck(c_mutex);
  BOOST_CHECK( stopped.timed_wait(lck, time_wait) );
}

void stop_test_server_st()
{
  stop_test_server(1);
}

void stop_test_server_mt(unsigned fnum)
{
  BOOST_CHECKPOINT(fnum);
  stop_test_server(16);
}

bool init_tests()
{
  test_suite& test = framework::master_test_suite();
  test.add( BOOST_TEST_CASE(&stop_test_server_st) );

  for (unsigned i = 0; i < 50; ++i)
    test.add( BOOST_TEST_CASE(boost::bind(stop_test_server_mt, i)));

  return true;
}

int main(int argc, char* argv[])
{
  boost::unit_test::unit_test_main( &init_tests, argc, argv );
}

// vim:ts=2:sw=2:et
