#include <boost/test/test_tools.hpp>
#include "libiqxmlrpc/thread.h"
#include "libiqxmlrpc/executor.h"
#include "libiqxmlrpc/http_server.h"
#include "libiqxmlrpc/https_server.h"
#include "server_classes.h"
#include "methods.h"

void serverctl_stop::execute( 
  const iqxmlrpc::Param_list&, iqxmlrpc::Value& )
{
  BOOST_MESSAGE("Stop_server method invoked.");
  server().log_message( "Stopping the server." );
  server().set_exit_flag();
}

// ----------------------------------------------------------------------------
Test_server::Test_server(const Test_server_config& conf):
  conf_(conf),
  impl_(conf_.port, conf_.exec_factory)
{
  impl_.log_errors( &std::cerr );
  impl_.enable_introspection();
  impl_.register_method<serverctl_stop>( "serverctl.stop" );

  register_user_methods(impl());
}

void Test_server::work()
{
  if (conf_.use_ssl)
    impl_.work<iqxmlrpc::Https_server_connection>();
  else
    impl_.work<iqxmlrpc::Http_server_connection>();
}
