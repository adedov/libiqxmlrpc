lib_LTLIBRARIES = libiqxmlrpc.la

libiqxmlrpc_la_LDFLAGS = -version-info @LIBIQXMLRPC_VERSION@
libiqxmlrpc_la_LIBADD =  @LIBXMLPP@ @OPENSSL@

pkginclude_HEADERS = \
  libiqxmlrpc.h \
  sysinc.h \
  conn_fabric.h \
  net_except.h \
  inet_addr.h \
  acceptor.h \
  connector.h \
  lock.h \
  reactor.h \
  connection.h \
  ssl_lib.h \
  ssl_connection.h \
  except.h \
  value_type.h \
  value.h \
  parser.h \
  parser_specific.h \
  request.h \
  response.h \
  method.h \
  executor.h \
  http.h \
  server.h \
  client.h \
  http_transport.h \
  https_transport.h

#  xml_util.h


libiqxmlrpc_la_SOURCES = \
  inet_addr.cc \
  net_except.cc \
  acceptor.cc \
  connector.cc \
  reactor.cc \
  connection.cc \
  ssl_lib.cc \
  ssl_connection.cc \
  except.cc \
  value_type.cc \
  value.cc \
  parser.cc \
  parser_specific.cc \
  request.cc \
  response.cc \
  method.cc \
  executor.cc \
  http.cc \
  server.cc \
  client.cc \
  http_transport.cc \
  https_transport.cc

#  xml_util.cc
