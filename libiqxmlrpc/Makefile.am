lib_LTLIBRARIES = libiqxmlrpc.la

libiqxmlrpc_la_LDFLAGS = -version-info @LIBRARY_VERSION@

pkginclude_HEADERS = \
  except.h \
  value_type.h \
  value.h \
  response.h \
  server.h

libiqxmlrpc_la_SOURCES = \
  except.cc \
  value_type.cc \
  value.cc \
  parser.h		parser.cc \
  parser_specific.h	parser_specific.cc \
  request.h		request.cc \
  response.cc \
  server.cc
