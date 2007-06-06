#include <memory>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/utility.hpp>
#include "libiqxmlrpc/libiqxmlrpc.h"
#include "libiqxmlrpc/http_server.h"
#include "libiqxmlrpc/executor.h"

using namespace boost::unit_test_framework;
using namespace iqxmlrpc;

class TestServer: boost::noncopyable {
public:
  TestServer(int port, unsigned threads):
    serv_(new Http_server(port, threads > 1 ?
      static_cast<Executor_factory_base*>(new Pool_executor_factory(threads)) :
      static_cast<Executor_factory_base*>(new Serial_executor_factory))),
    thread_(new boost::thread(boost::bind(&TestServer::run, this)))
  {
  }

  void stop()
  {
    serv_->set_exit_flag();
  }

  void join()
  {
    thread_->join();
  }

private:
  boost::scoped_ptr<iqxmlrpc::Server> serv_;
  boost::scoped_ptr<boost::thread> thread_;

  static void run(TestServer* obj)
  {
    obj->serv_->work();
  }
};

void stop_and_join(unsigned threads, boost::condition* on_stop)
{
  TestServer s(3344, threads);

  boost::xtime time_wait;
  boost::xtime_get(&time_wait, boost::TIME_UTC);
  time_wait.sec += 1;

  boost::thread::sleep(time_wait);

  s.stop();
  s.join();
  on_stop->notify_all();
}

void stop_test_server(unsigned server_threads)
{
  boost::condition stopped;
  boost::mutex c_mutex;

  boost::thread thr(boost::bind(stop_and_join, server_threads, &stopped));

  boost::xtime time_wait;
  boost::xtime_get(&time_wait, boost::TIME_UTC);
  time_wait.sec += 5;

  boost::mutex::scoped_lock lck(c_mutex);
  BOOST_CHECK( stopped.timed_wait(lck, time_wait) );
}

void stop_test_server_st()
{
  stop_test_server(1);
}

void stop_test_server_mt()
{
  stop_test_server(10);
}

test_suite* init_unit_test_suite(int argc, char* argv[])
{
  try {
    test_suite* test = BOOST_TEST_SUITE("Stopping server test");
    test->add( BOOST_TEST_CASE(&stop_test_server_st) );
    test->add( BOOST_TEST_CASE(&stop_test_server_mt) );
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
  catch(...)
  {
    std::cerr << "Unexpected exception" << std::endl;
    throw;
  }
}

// vim:ts=2:sw=2:et
