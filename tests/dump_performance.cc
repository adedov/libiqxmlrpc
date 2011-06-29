#include <iostream>
#include "libiqxmlrpc/response.h"
#include "libiqxmlrpc/value.h"

using namespace iqxmlrpc;

void dump_test()
{
  Array arr;

  for (int i = 0; i < 100000; ++i) {
    Struct s;
    s.insert("f1", "field 1");
    s.insert("f2", "field 2");
    s.insert("f3", i);
    arr.push_back(s);
  }

  Value *v = new Value(arr);

  time_t t1, t2;
  time(&t1);
  Response r(v);
  std::string s = r.dump_xml(true);
  time(&t2);

  std::cout << "Spent time " << t2 - t1 << std::endl;

  std::cerr << s << std::flush;
}

int main(int argc, char* argv[])
{
  dump_test();
  return 0;
}
