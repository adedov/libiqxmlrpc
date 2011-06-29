#include <algorithm>
#include <iostream>
#include <iterator>
#include "libiqxmlrpc/value.h"
#include "libiqxmlrpc/response.h"

using namespace iqxmlrpc;

void parse_test(const std::string& s)
{
  time_t t1, t2;
  time(&t1);
  parse_response(s);
  time(&t2);

  std::cout << "Spent time " << t2 - t1 << std::endl;
}

int main(int argc, char* argv[])
{
  std::string s;
  std::copy(
      std::istreambuf_iterator<char>(std::cin.rdbuf()),
      std::istreambuf_iterator<char>(),
      std::back_inserter(s));
  parse_test(s);
  return 0;
}
