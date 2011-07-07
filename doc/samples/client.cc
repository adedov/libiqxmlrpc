#include <iostream>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/http_client.h>

int main()
{
  using namespace iqxmlrpc;

  Client<Http_client_connection> client(iqnet::Inet_addr(3344));

  Param_list pl;
  pl.push_back(Struct());
  pl[0].insert("var1", 1);
  pl[0].insert("var2", "value");

  Response r = client.execute("echo", pl);

  assert(r.value()["var1"].get_int() == 1);
  std::cout << "OK" << std::endl;
}
