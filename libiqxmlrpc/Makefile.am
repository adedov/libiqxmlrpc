lib_LTLIBRARIES = libiqxmlrpc.la

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
  executor.h \
  http.h \
  server.h \
  client.h \
  http_transport.h \
  https_transport.h

#  xml_util.h


libiqxmlrpc_la_SOURCES = \
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
  
libiqxmlrpc_la_LDFLAGS = -version-info @LIBIQXMLRPC_VERSION@
libiqxmlrpc_la_LIBADD =  @LIBXMLPP@ ../libiqnet/libiqnet.la 
