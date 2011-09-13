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

set(PUBLIC_HEADERS
  acceptor.h
  api_export.h
  auth_plugin.h
  builtins.h
  client.h
  client_conn.h
  client_opts.h
  connection.h
  connector.h
  conn_factory.h
  dispatcher_manager.h
  except.h
  executor.h
  firewall.h
  http.h
  http_errors.h
  http_client.h
  http_server.h
  https_client.h
  https_server.h
  inet_addr.h
  libiqxmlrpc.h
  lock.h
  method.h
  net_except.h
  reactor.h
  reactor_interrupter.h
  request.h
  response.h
  server_conn.h
  server.h
  socket.h
  ssl_lib.h
  ssl_connection.h
  sysinc.h
  util.h
  value_type.h
  value_type_visitor.h
  value.h
)

set(PRIVATE_HEADERS
  parser2.h
  value_parser.h
  request_parser.h
  response_parser.h
  reactor_impl.h
  reactor_poll_impl.h
  reactor_select_impl.h
  value_type_xml.h
  xml_builder.h
)

add_library(iqxmlrpc SHARED
  ${PUBLIC_HEADERS}
  ${PRIVATE_HEADERS}
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

install(TARGETS iqxmlrpc LIBRARY DESTINATION lib)
install(FILES ${PUBLIC_HEADERS} DESTINATION include)
