lib_LTLIBRARIES = libiqxmlrpc.la libiqxmlrpc-net.la

pkginclude_HEADERS = \
  libiqxmlrpc.h \
  except.h \
  value_type.h \
  value.h \
  parser.h \
  parser_specific.h \
  request.h \
  response.h \
  method.h \
  null_transport.h \
  http.h \
  http_transport.h


libiqxmlrpc_la_LDFLAGS = -version-info @LIBIQXMLRPC_VERSION@
libiqxmlrpc_la_LIBADD = @LIBXMLPP@

libiqxmlrpc_la_SOURCES = \
  except.cc \
  value_type.cc \
  value.cc \
  parser.cc \
  parser_specific.cc \
  request.cc \
  response.cc \
  method.cc \
  null_transport.cc \
  http.cc


libiqxmlrpc_net_la_SOURCES = \
  http_transport.cc
  
libiqxmlrpc_net_la_LDFLAGS = -version-info @LIBIQXMLRPC_VERSION@
libiqxmlrpc_net_la_LIBADD = libiqxmlrpc.la ../libiqnet/libiqnet.la
