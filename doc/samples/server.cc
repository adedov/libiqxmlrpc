#include <iostream>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/http_server.h>

// Simple method that just returns back first input parameter
class Echo: public iqxmlrpc::Method {
public:
  void execute(const iqxmlrpc::Param_list& params, iqxmlrpc::Value& retval)
  {
    if (params.empty())
      retval = 0;
    else
      retval = params[0];
  }
};

int main()
{
  int port = 3344;

  iqxmlrpc::Serial_executor_factory ef;
  iqxmlrpc::Http_server server(port, &ef);

  iqxmlrpc::register_method<Echo>(server, "echo");

  // optional settings
  server.log_errors( &std::cerr );
  server.enable_introspection();
  server.set_max_request_sz(1024*1024);

  // start server
  server.work();
}
