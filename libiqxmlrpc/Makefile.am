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
  thread.h \
  reactor.h \
  connection.h \
  sigsock.h \
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
  http_client.h \
  http_server.h \
  https_client.h \
  https_server.h

#  xml_util.h


libiqxmlrpc_la_SOURCES = \
  inet_addr.cc \
  net_except.cc \
  acceptor.cc \
  connector.cc \
  reactor.cc \
  connection.cc \
  lock.cc \
  thread.cc \
  sigsock.cc \
  ssl_lib.cc \
  ssl_connection.cc \
  except.cc \
  value_type.cc \
  value.cc \
  parser.cc \
  parser_specific.cc \
  request.cc \
  response.cc \
  http.cc \
  http_client.cc \
  http_server.cc \
  https_client.cc \
  https_server.cc \
  method.cc \
  executor.cc \
  server.cc \
  client.cc

#  xml_util.cc

# Server only sources:
#  thread.*
#  sigsock.*
#  method.*
#  executor.*
#  server.*
#  http_server.*
#  https_server.*
#  
# Client only sources:
#  http_client.*
#  https_client.*
#  client.*

clean-local:
	rm -f *.da  