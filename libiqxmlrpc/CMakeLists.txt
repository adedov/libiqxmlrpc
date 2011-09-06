include(FindBoost)
include(FindPkgConfig)
include(CheckFunctionExists)

find_package(Boost 1.41.0 COMPONENTS date_time thread)
pkg_check_modules(XML2 REQUIRED libxml-2.0)
pkg_check_modules(OPENSSL REQUIRED openssl)
check_function_exists(poll HAVE_POLL)

configure_file(config.h.in config.h)
configure_file(version.h.in version.h)
include_directories(${XML2_INCLUDE_DIRS})

if(${HAVE_POLL})
	set(REACTOR_IMPL "poll")
else(${HAVE_POLL})
	set(REACTOR_IMPL "select")
endif(${HAVE_POLL})
message("Using ${REACTOR_IMPL} reactor implementation")

add_library(iqxmlrpc SHARED
  acceptor.cc
  auth_plugin.cc
  builtins.cc
  client.cc
  client_conn.cc
  connection.cc
  connector.cc
  dispatcher_manager.cc
  executor.cc
  http.cc
  http_client.cc
  https_client.cc
  http_server.cc
  https_server.cc
  inet_addr.cc
  method.cc
  net_except.cc
  parser2.cc
  reactor_interrupter.cc
  reactor_${REACTOR_IMPL}_impl.cc
  request.cc
  request_parser.cc
  response.cc
  response_parser.cc
  server.cc
  server_conn.cc
  socket.cc
  ssl_connection.cc
  ssl_lib.cc
  value.cc
  value_parser.cc
  value_type.cc
  value_type_visitor.cc
  value_type_xml.cc
  xml_builder.cc
)

target_link_libraries(iqxmlrpc
  ${Boost_LIBRARIES}
  ${XML2_LIBRARIES}
  ${OPENSSL_LIBRARIES}
)
