#include <boost/test/test_tools.hpp>
#include "libiqxmlrpc/thread.h"
#include "libiqxmlrpc/executor.h"
#include "libiqxmlrpc/http_server.h"
#include "libiqxmlrpc/https_server.h"
#include "server_classes.h"
#include "methods.h"

class Server_thread: public iqnet::Thread {
  Test_server_config conf_;

public:
  Server_thread(const Test_server_config& conf):
    conf_(conf) {}

protected:
  void do_run()
  {
    User_test_server serv(conf_);
    serv.work();
  }
};

// ----------------------------------------------------------------------------
void serverctl_stop::execute( 
  const iqxmlrpc::Param_list&, iqxmlrpc::Value& )
{
  BOOST_MESSAGE("Stop_server method invoked.");
  server().log_message( "Stopping the server." );
  server().set_exit_flag();
}

// ----------------------------------------------------------------------------
void serverctl_start::execute( 
  const iqxmlrpc::Param_list& params, iqxmlrpc::Value& retval )
{
  BOOST_MESSAGE("Start_server method invoked.");
  server().log_message( "Starting the server." );
  Test_server_config conf = Test_server_config::create(params.front());
  Server_thread* sthr = new Server_thread(conf);
}

// ----------------------------------------------------------------------------
Test_server_base::Test_server_base(const Test_server_config& conf):
  conf_(conf),
  impl_(conf_.port, conf_.exec_factory)
{
  impl_.log_errors( &std::cerr );
  impl_.register_method<serverctl_stop>( "serverctl.stop" );
}

void Test_server_base::work()
{
  if (conf_.use_ssl)
    impl_.work<iqxmlrpc::Https_server_connection>();
  else
    impl_.work<iqxmlrpc::Http_server_connection>();
}

// ----------------------------------------------------------------------------
Test_suite_server::Test_suite_server(const Test_server_config& conf):
  Test_server_base(conf)
{
  impl().register_method<serverctl_start>( "serverctl.start" );
}

// ----------------------------------------------------------------------------
User_test_server::User_test_server(const Test_server_config& conf):
  Test_server_base(conf)
{
  register_user_methods(impl());
}
