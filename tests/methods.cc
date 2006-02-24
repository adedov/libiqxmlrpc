#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <openssl/md5.h>
#include <boost/test/test_tools.hpp>
#include "libiqxmlrpc/server.h"
#include "methods.h"

using namespace iqxmlrpc;

void register_user_methods(iqxmlrpc::Server& s)
{
  register_method<serverctl_stop>(s, "serverctl.stop");
  register_method(s, "echo", echo_method);
  register_method<Get_file>(s, "get_file");
}

void serverctl_stop::execute( 
  const iqxmlrpc::Param_list&, iqxmlrpc::Value& )
{
  BOOST_MESSAGE("Stop_server method invoked.");
  server().log_message( "Stopping the server." );
  server().set_exit_flag();
}


void echo_method(
  iqxmlrpc::Method*,
  const iqxmlrpc::Param_list& args,
  iqxmlrpc::Value& retval )
{
  BOOST_MESSAGE("Echo method invoked.");
  retval = args[0];
}

namespace 
{
  inline char brand()
  {
    return rand()%255;
  }
}

void Get_file::execute( 
  const iqxmlrpc::Param_list& args, iqxmlrpc::Value& retval )
{
  BOOST_MESSAGE("Get_file method invoked.");

  int retsize = args[0]["requested-size"]; 
  if (retsize <= 0)
	  throw Fault( 0, "requested-size must be > 0" );

  BOOST_MESSAGE("Generating data...");
  srand(time(0));
  std::string s(retsize, '\0');
  std::generate(s.begin(), s.end(), brand);

  retval = Struct();
  retval.insert("data", Binary_data::from_data(s));

  BOOST_MESSAGE("Calculating MD5 checksum...");
  typedef const unsigned char md5char;
  typedef const char strchar;

  unsigned char md5[16];
  MD5(reinterpret_cast<md5char*>(s.data()), s.length(), md5);
  
  retval.insert("md5", Binary_data::from_data(
    reinterpret_cast<strchar*>(md5), sizeof(md5)));
}
