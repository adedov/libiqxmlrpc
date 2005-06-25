#ifndef _iqxmlrpc_test_suite_client_common_h_
#define _iqxmlrpc_test_suite_client_common_h_

#include <string>
#include <libiqxmlrpc/client.h>

class Method_proxy {
  iqxmlrpc::Client_base* client_;
  std::string method_name_;

public:
  Method_proxy(iqxmlrpc::Client_base*, const std::string& name);
  virtual ~Method_proxy();

  iqxmlrpc::Response operator ()(const iqxmlrpc::Value&);
};

class Start_server_proxy: public Method_proxy {
public:
  Start_server_proxy(iqxmlrpc::Client_base* cb):
    Method_proxy(cb, "serverctl.start") {}

  iqxmlrpc::Response operator ()(int port, bool use_ssl, int numthreads)
  {
    iqxmlrpc::Struct param;
    param.insert("port", port);
    param.insert("use-ssl", use_ssl);
    param.insert("numthreads", numthreads);

    return Method_proxy::operator ()(param);
  }
};

class Stop_server_proxy: public Method_proxy {
public:
   Stop_server_proxy(iqxmlrpc::Client_base* cb):
     Method_proxy(cb, "serverctl.stop") {}

   iqxmlrpc::Response operator ()()
  {
    return Method_proxy::operator ()(iqxmlrpc::Nil());
  }
};

class Introspection_proxy: public Method_proxy {
public:
  Introspection_proxy(iqxmlrpc::Client_base* cb):
    Method_proxy(cb, "system.listMethods") {}

  iqxmlrpc::Response operator ()()
  {
    return Method_proxy::operator ()(iqxmlrpc::Nil());
  }
};

class Echo_proxy: public Method_proxy {
public:
  Echo_proxy(iqxmlrpc::Client_base* cb):
    Method_proxy(cb, "echo") {}
};

class Get_file_proxy: public Method_proxy {
public:
  Get_file_proxy(iqxmlrpc::Client_base* cb):
    Method_proxy(cb, "get_file") {}

  iqxmlrpc::Response operator ()(int size)
  {
    iqxmlrpc::Struct s;
    s.insert("requested-size", size);
    return Method_proxy::operator ()(s);
  }
};

#endif
