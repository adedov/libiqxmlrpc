LDFLAGS += -version-info @LIBIQXMLRPC_VERSION@

noinst_LTLIBRARIES = libiqxmlrpc-general.la
libiqxmlrpc_general_la_LIBADD = ${LIBXMLPP_LIBS} ${OPENSSL_LIBS}

lib_LTLIBRARIES = libiqxmlrpc-client.la libiqxmlrpc-server.la libiqxmlrpc.la

libiqxmlrpc_client_la_LIBADD = libiqxmlrpc-general.la
libiqxmlrpc_server_la_LIBADD = libiqxmlrpc-general.la

libiqxmlrpc_la_SOURCES = libiqxmlrpc.h
libiqxmlrpc_la_LIBADD = libiqxmlrpc-general.la libiqxmlrpc-client.la libiqxmlrpc-server.la

EXTRA_DIST = version.h.in version.h 

pkginclude_HEADERS = \
  libiqxmlrpc.h \
  sysinc.h \
  conn_factory.h \
  net_except.h \
  inet_addr.h \
  socket.h \
  acceptor.h \
  connector.h \
  lock.h \
  mt_synch.h \
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
  builtins.h \
  executor.h \
  http.h \
  server.h \
  client.h \
  http_client.h \
  http_server.h \
  https_client.h \
  https_server.h \
  utf_conv.h

#  xml_util.h


libiqxmlrpc_general_la_SOURCES = \
  inet_addr.cc \
  net_except.cc \
  socket.cc \
  acceptor.cc \
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
  http.cc \
  utf_conv.cc

libiqxmlrpc_client_la_SOURCES = \
  http_client.cc \
  https_client.cc \
  client.cc
  
libiqxmlrpc_server_la_SOURCES = \
  mt_synch.cc \
  thread.cc \
  sigsock.cc \
  http_server.cc \
  https_server.cc \
  method.cc \
  builtins.cc \
  executor.cc \
  server.cc

#  xml_util.cc

# Server only sources:
#  thread.*
#  sigsock.*
#  method.*
#  builtins.*
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