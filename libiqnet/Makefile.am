lib_LTLIBRARIES = libiqnet.la

#CXXFLAGS = -g
#LDFLAGS = -g
libiqnet_la_LDFLAGS = -version-info @LIBIQNET_VERSION@
libiqnet_la_LIBADD = @OPENSSL@

iqnetincdir = ${includedir}/libiqnet
iqnetinc_HEADERS = \
  conn_fabric.h \
  net_except.h \
  inet_addr.h \
  acceptor.h \
  connector.h \
  reactor.h \
  connection.h \
  mt.h \
  ssl_lib.h \
  ssl_connection.h

libiqnet_la_SOURCES = \
  inet_addr.cc \
  acceptor.cc \
  connector.cc \
  reactor.cc \
  connection.cc \
  mt.cc \
  ssl_lib.cc \
  ssl_connection.cc
