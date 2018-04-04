#define BOOST_TEST_MODULE xheaders_test

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "libiqxmlrpc/xheaders.h"

using namespace boost::unit_test;
using namespace iqxmlrpc;

class XHeaders_fixture {
public:
  typedef std::map<std::string, std::string> Headers;
  Headers opt;
  XHeaders h;
public:
  XHeaders_fixture() {
    opt["X-id"] = "1";
    opt["x-cmd"] = "2";
    opt["Xcmd"] = "3";
    opt["xmesg"] = "4";
    opt["content_length"] = "5";
  }
};

BOOST_AUTO_TEST_CASE( validate ) {
  BOOST_CHECK(XHeaders::validate("X-d"));
  BOOST_CHECK(XHeaders::validate("x-d"));
  BOOST_CHECK(!XHeaders::validate("Xd"));
  BOOST_CHECK(!XHeaders::validate("xd"));
  BOOST_CHECK(!XHeaders::validate("data"));
}

BOOST_FIXTURE_TEST_CASE( find, XHeaders_fixture )
{
  BOOST_TEST_MESSAGE("Operator = test...");

  h = opt;

  BOOST_CHECK_EQUAL(h.size(), 2);
  BOOST_CHECK(h.find("X-id") != h.end());
  BOOST_CHECK(h.find("x-id") != h.end());
  BOOST_CHECK(h.find("X-cmd") != h.end());
  BOOST_CHECK(h.find("x-cmd") != h.end());

  std::string res;
  for (XHeaders::const_iterator it = h.begin(); it != h.end(); ++it) {
    res+=it->first;
    res+=it->second;
  }
  BOOST_CHECK(res == "x-id1x-cmd2" || res == "x-cmd2x-id1");
}

BOOST_FIXTURE_TEST_CASE( brackets, XHeaders_fixture )
{
  BOOST_TEST_MESSAGE("Operator = test...");

  h["X-new"] = "new";
  BOOST_CHECK_EQUAL(h.size(), 1);
  BOOST_CHECK(h.find("X-new") != h.end());
  BOOST_CHECK(h.find("x-new") != h.end());
  BOOST_CHECK(h.find("X-new1") == h.end());

  BOOST_CHECK_THROW(h["length"], Error_xheader);

  BOOST_CHECK_EQUAL(h["X-new"], "new");
  BOOST_CHECK_EQUAL(h["x-new"], "new");
}

// vim:ts=2:sw=2:et
