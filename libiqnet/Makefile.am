lib_LTLIBRARIES = libiqnet.la

libiqnet_la_LDFLAGS = -version-info @LIBIQNET_VERSION@
libiqnet_la_LIBADD = @OPENSSL@

iqnetincdir = ${includedir}/libiqnet
iqnetinc_HEADERS = \
  conn_fabric.h \
  net_except.h \
  inet_addr.h \
  acceptor.h \
  connector.h \
  lock.h \
  reactor.h \
  connection.h \
  ssl_lib.h \
  ssl_connection.h

libiqnet_la_SOURCES = \
  inet_addr.cc \
  net_except.cc \
  acceptor.cc \
  connector.cc \
  reactor.cc \
  connection.cc \
  ssl_lib.cc \
  ssl_connection.cc
