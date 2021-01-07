#define BOOST_TEST_MODULE test_parser
#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "libiqxmlrpc/value.h"
#include "libiqxmlrpc/value_parser.h"
#include "libiqxmlrpc/request_parser.h"
#include "libiqxmlrpc/response_parser.h"

using namespace boost::unit_test;
using namespace iqxmlrpc;

//
// values
//

Value parse_value(const std::string& s)
{
  Parser p(s);
  ValueBuilder b(p);
  b.build();
  return Value(b.result());
}

BOOST_AUTO_TEST_CASE(test_parse_scalar)
{
  BOOST_CHECK_EQUAL(parse_value("<int>123</int>").get_int(), 123);
  BOOST_CHECK_EQUAL(parse_value("<i4>123</i4>").get_int(), 123);
  BOOST_CHECK_EQUAL(parse_value("<i8>5000000000</i8>").get_int64(), 5000000000L);
  BOOST_CHECK_EQUAL(parse_value("<boolean>0</boolean>").get_bool(), false);
  BOOST_CHECK_EQUAL(parse_value("<double>123.45</double>").get_double(), 123.45);
  BOOST_CHECK_EQUAL(parse_value("<string>str</string>").get_string(), "str");
  BOOST_CHECK_EQUAL(parse_value("<base64>V2h5IHNob3VsZCBJIGJsYW1lIGhlcg==</base64>").get_binary().get_data(), "Why should I blame her");
  BOOST_CHECK_EQUAL(parse_value("<dateTime.iso8601>19980717T14:08:55</dateTime.iso8601>").get_datetime().to_string(), "19980717T14:08:55");
  BOOST_CHECK(parse_value("<nil/>").is_nil());
}

BOOST_AUTO_TEST_CASE(test_parse_array)
{
  Array v = parse_value(
    "<array>"
      "<data>"
      "<value><i4>123</i4></value>"
      "<value><double>123.456</double></value>"
      "<value><string>str</string></value>"
      "<value>str2</value>"
      "<value><boolean>1</boolean></value>"
      "<value>"
          "<struct>"
          "<member><name>v1</name><value>str</value></member>"
          "<member><name>v2</name><value><int>123</int></value></member>"
          "<member><name>v3</name><value><i8>6000000000</i8></value></member>"
          "</struct>"
      "</value>"
      "<value><i8>5000000000</i8></value>"
      "</data>"
    "</array>").the_array();

  BOOST_CHECK_EQUAL(v.size(), 7);
  BOOST_CHECK_EQUAL(v[0].get_int(), 123);
  BOOST_CHECK_EQUAL(v[1].get_double(), 123.456);
  BOOST_CHECK_EQUAL(v[2].get_string(), "str");
  BOOST_CHECK_EQUAL(v[3].get_string(), "str2");
  BOOST_CHECK_EQUAL(v[4].get_bool(), true);
  BOOST_CHECK(v[5].is_struct());
  BOOST_CHECK_EQUAL(v[5].the_struct().size(), 3);
  BOOST_CHECK_EQUAL(v[5].the_struct()["v1"].get_string(), "str");
  BOOST_CHECK_EQUAL(v[5].the_struct()["v2"].get_int(), 123);
  BOOST_CHECK_EQUAL(v[5].the_struct()["v3"].get_int64(), 6000000000l);
  BOOST_CHECK_EQUAL(v[6].get_int64(), 5000000000);
}

BOOST_AUTO_TEST_CASE(test_parse_unknown_type)
{
  BOOST_CHECK_THROW(parse_value("<abc>0</abc>"), XML_RPC_violation);
}

BOOST_AUTO_TEST_CASE(test_parse_bad_xml)
{
  BOOST_CHECK_THROW(parse_value("not valid <xml>"), Parse_error);
  BOOST_CHECK_THROW(parse_value("<doc></abc></doc>"), Parse_error);
}

BOOST_AUTO_TEST_CASE(test_parse_simple_struct)
{
  Struct s = parse_value(
    "<struct>"
      "<member><name>v1</name><value>str</value></member>"
      "<member><name>v2</name><value><int>123</int></value></member>"
      "<member><name>v3</name><value><string>str2</string></value></member>"
      "<member><name/><value><string>str2</string></value></member>"
    "</struct>").the_struct();

  BOOST_CHECK_EQUAL(s.size(), 4);
  BOOST_CHECK_EQUAL(s["v1"].get_string(), "str");
  BOOST_CHECK_EQUAL(s["v2"].get_int(), 123);
  BOOST_CHECK_EQUAL(s["v3"].get_string(), "str2");
  BOOST_CHECK_EQUAL(s[""].get_string(), "str2");
}

BOOST_AUTO_TEST_CASE(test_parse_nested_struct)
{
  Struct s = parse_value(
    "<struct>"
      "<member>"
        "<name>nested</name>"
        "<value>"
          "<struct>"
            "<member><name>v1</name><value>str</value></member>"
            "<member><name>v2</name><value><int>123</int></value></member>"
            "<member><name>v3</name><value><string>str2</string></value></member>"
          "</struct>"
        "</value>"
      "</member>"
      "</struct>").the_struct();

  BOOST_CHECK_EQUAL(s.size(), 1);
  BOOST_CHECK(s["nested"].is_struct());
  Struct s2 = s["nested"].the_struct();
  BOOST_CHECK_EQUAL(s2["v1"].get_string(), "str");
  BOOST_CHECK_EQUAL(s2["v2"].get_int(), 123);
  BOOST_CHECK_EQUAL(s2["v3"].get_string(), "str2");
}

BOOST_AUTO_TEST_CASE(test_parse_formatted)
{
  Value v = parse_value(" \
<struct> \
  <member> \
    <name>name</name> \
    <value> \
      <int>123</int> \
    </value> \
  </member> \
</struct>");

  BOOST_CHECK(v.is_struct());
}

BOOST_AUTO_TEST_CASE(test_parse_emptiness)
{
  BOOST_CHECK_THROW(parse_value("<double></double>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<double/>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<boolean></boolean>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<boolean/>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<datetime.iso8601></datetime.iso8601>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<dateTime.iso8601/>"), XML_RPC_violation);

  Value::set_default_int(-123);
  Value::set_default_int64(-1234567890L);
  BOOST_CHECK_EQUAL(parse_value("<int></int>").get_int(), -123);
  BOOST_CHECK_EQUAL(parse_value("<int/>").get_int(), -123);
  BOOST_CHECK_EQUAL(parse_value("<i4></i4>").get_int(), -123);
  BOOST_CHECK_EQUAL(parse_value("<i4/>").get_int(), -123);
  BOOST_CHECK_EQUAL(parse_value("<i8></i8>").get_int64(), -1234567890L);
  BOOST_CHECK_EQUAL(parse_value("<i8/>").get_int64(), -1234567890L);

  Value::drop_default_int();
  Value::drop_default_int64();
  BOOST_CHECK_THROW(parse_value("<int></int>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<int/>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<i4></i4>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<i4/>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<i8></i8>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<i8/>"), XML_RPC_violation);

  BOOST_CHECK_EQUAL(parse_value("<string/>").get_string(), "");
  BOOST_CHECK_EQUAL(parse_value("<string></string>").get_string(), "");
  BOOST_CHECK_EQUAL(parse_value("<base64/>").get_binary().get_data(), "");
  BOOST_CHECK_EQUAL(parse_value("<base64></base64>").get_binary().get_data(), "");

  //
  // Arrays with empty values
  BOOST_CHECK_EQUAL(parse_value("<array/>").the_array().size(), 0);
  BOOST_CHECK_EQUAL(parse_value("<array></array>").the_array().size(), 0);
  BOOST_CHECK_EQUAL(parse_value("<array><data></data></array>").the_array().size(), 0);

  Array a1 = parse_value("<array><data><value/></data></array>").the_array();
  BOOST_CHECK_EQUAL(a1.size(), 1);
  BOOST_CHECK_EQUAL(a1[0].get_string(), "");

  Array a2 = parse_value("<array><data><value></value></data></array>").the_array();
  BOOST_CHECK_EQUAL(a2.size(), 1);
  BOOST_CHECK_EQUAL(a2[0].get_string(), "");

  Array a3 = parse_value("<array><data><value><struct/></value><value><struct/></value></data></array>").the_array();
  BOOST_CHECK_EQUAL(a3.size(), 2);
  BOOST_CHECK_EQUAL(a3[0].the_struct().size(), 0);
  BOOST_CHECK_EQUAL(a3[1].the_struct().size(), 0);

  //
  // Structs with empty values
  BOOST_CHECK_EQUAL(parse_value("<struct/>").the_struct().size(), 0);
  BOOST_CHECK_EQUAL(parse_value("<struct></struct>").the_struct().size(), 0);
  BOOST_CHECK_THROW(parse_value("<struct><member></member></struct>").the_struct(), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<struct><member><value/></member></struct>").the_struct(), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<struct><member><name/></member></struct>").the_struct(), XML_RPC_violation);

  Struct s1 = parse_value("<struct><member><name/><value>123</value></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s1.size(), 1);
  BOOST_CHECK_EQUAL(s1[""].get_string(), "123");

  Struct s2 = parse_value("<struct><member><name></name><value/></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s2.size(), 1);
  BOOST_CHECK_EQUAL(s2[""].get_string(), "");

  Struct s3 = parse_value("<struct><member><name></name><value></value></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s3.size(), 1);
  BOOST_CHECK_EQUAL(s3[""].get_string(), "");

  Struct s4 = parse_value("<struct><member><name/><value></value></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s4.size(), 1);
  BOOST_CHECK_EQUAL(s4[""].get_string(), "");

  Struct s5 = parse_value("<struct><member><name/><value/></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s5.size(), 1);
  BOOST_CHECK_EQUAL(s5[""].get_string(), "");

  Struct s6 = parse_value("<struct><member><name>var1</name><value><array><data><value><struct></struct></value></data></array></value></member></struct>").the_struct();
  BOOST_CHECK(s6.has_field("var1"));
  print_value(s6, std::cout);

  Struct s7 = parse_value("<struct><member><name>var1</name><value><array><data><value><struct/></value></data></array></value></member></struct>").the_struct();
  BOOST_CHECK(s7.has_field("var1"));
  print_value(s7, std::cout);
}

BOOST_AUTO_TEST_CASE(test_malfromed_value)
{
  BOOST_CHECK_THROW(parse_value("<struct><member><name><MALFORMED>abc</MALFORMED></name><value>OK</value></member></struct>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<struct><member><name>OK</name><value><MALFORMED>OK</MALFORMED></value></member></struct>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<string><MALFORMED>OK</MALFORMED></string>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<int><MALFORMED>123</MALFORMED></int>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<int><MALFORMED/>123</int>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<int>123<MALFORMED/></int>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<i8><MALFORMED>123</MALFORMED></i8>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<string>OK<MALFORMED/></string>"), XML_RPC_violation);
}

//
// request
//

BOOST_AUTO_TEST_CASE(test_parse_request)
{
  std::string r = "<methodCall> \
  <methodName>get_weather</methodName> \
  <params> \
    <param> \
      <value> \
        <string>Krasnoyarsk</string> \
      </value> \
    </param> \
    <param> \
      <value>now</value> \
    </param> \
  </params> \
</methodCall>";

  std::auto_ptr<Request> req(parse_request(r));
  BOOST_CHECK_EQUAL(req->get_name(), "get_weather");
  BOOST_CHECK_EQUAL(req->get_params().size(), 2);
  BOOST_CHECK_EQUAL(req->get_params().front().get_string(), "Krasnoyarsk");
  BOOST_CHECK_EQUAL(req->get_params().back().get_string(), "now");
}

BOOST_AUTO_TEST_CASE(test_parse_request_empty_param)
{
  std::string r = "<methodCall> \
  <methodName>do_something</methodName> \
  <params> \
    <param> \
      <value> \
      </value> \
    </param> \
  </params> \
</methodCall>";

  std::auto_ptr<Request> req(parse_request(r));
  BOOST_CHECK_EQUAL(req->get_name(), "do_something");
  BOOST_CHECK_EQUAL(req->get_params().size(), 1);
  BOOST_CHECK_EQUAL(req->get_params().back().get_string(), "");
}

BOOST_AUTO_TEST_CASE(test_parse_request_no_params)
{
  std::string r = "<methodCall><methodName>do_something</methodName></methodCall>";
  std::auto_ptr<Request> req(parse_request(r));
  BOOST_CHECK_EQUAL(req->get_name(), "do_something");
  BOOST_CHECK_EQUAL(req->get_params().size(), 0);
}

BOOST_AUTO_TEST_CASE(test_parse_malformed_request)
{
  std::string r = "<methodCall><methodName><MALFORMED>do_something</MALFORMED></methodName></methodCall>";
  BOOST_CHECK_THROW(parse_request(r), XML_RPC_violation);

  // empty methodCall
  r = "<methodCall></methodCall>";
  BOOST_CHECK_THROW(parse_request(r), XML_RPC_violation);

  // method call without method name
  r = "<methodCall><params><param><value><i4>1</i4></value></param></params></methodCall>";
  BOOST_CHECK_THROW(parse_request(r), XML_RPC_violation);

  // method call with two method names
  r = "<methodCall><name>N1</name><params/><name>N2</name></methodCall>";
  BOOST_CHECK_THROW(parse_request(r), XML_RPC_violation);
  r = "<methodCall><name>N1</name><name>N2></name><params/></methodCall>";
  BOOST_CHECK_THROW(parse_request(r), XML_RPC_violation);

  // method call with multiple params
  r = "<methodCall><methodName>OK</methodName><params/><params/></methodCall>";
  BOOST_CHECK_THROW(parse_request(r), XML_RPC_violation);
}

//
// response
//

BOOST_AUTO_TEST_CASE(test_parse_empty_response)
{
  std::string r = "<methodResponse> \
  <params> \
    <param> \
      <value> \
      </value> \
    </param> \
  </params> \
</methodResponse>";

  Response res = parse_response(r);
  BOOST_CHECK_EQUAL(res.value().get_string(), "");
}

BOOST_AUTO_TEST_CASE(test_parse_ok_response)
{
  std::string r = "<methodResponse> \
  <params> \
    <param> \
      <value> \
       <struct> \
        <member> \
          <name>temp</name> \
          <value><double>15.5</double></value> \
        </member> \
       </struct> \
      </value> \
    </param> \
  </params> \
</methodResponse>";

  Response res = parse_response(r);
  BOOST_CHECK(res.value().is_struct());
  BOOST_CHECK_EQUAL(res.value().the_struct().size(), 1);
  BOOST_CHECK_EQUAL(res.value()["temp"].get_double(), 15.5);
}

BOOST_AUTO_TEST_CASE(test_parse_empty_fault_response_1)
{
  std::string r = "<methodResponse> \
    <fault> \
    </fault> \
</methodResponse>";

  BOOST_CHECK_THROW(parse_response(r), XML_RPC_violation);
}

BOOST_AUTO_TEST_CASE(test_parse_empty_fault_response_2)
{
  std::string r = "<methodResponse> \
    <fault> \
    <value> \
    </value> \
    </fault> \
</methodResponse>";

  BOOST_CHECK_THROW(parse_response(r), XML_RPC_violation);
}

BOOST_AUTO_TEST_CASE(test_parse_fault_response)
{
  std::string r = "<methodResponse> \
    <fault> \
      <value> \
       <struct> \
        <member> \
          <name>faultCode</name> \
          <value><int>143</int></value> \
        </member> \
        <member> \
          <name>faultString</name> \
          <value>Out of beer</value> \
        </member> \
       </struct> \
      </value> \
    </fault> \
</methodResponse>";

  Response res = parse_response(r);
  BOOST_CHECK(res.is_fault());
  BOOST_CHECK_EQUAL(res.fault_code(), 143);
  BOOST_CHECK_EQUAL(res.fault_string(), "Out of beer");
}

// vim:ts=2:sw=2:et
