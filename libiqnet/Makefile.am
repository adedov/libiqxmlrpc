lib_LTLIBRARIES = libiqnet.la

#CXXFLAGS = -g
#LDFLAGS = -g
libiqnet_la_LDFLAGS = -version-info @LIBIQNET_VERSION@
libiqnet_la_LIBADD = @OPENSSL@

libiqnet_la_SOURCES = \
  conn_fabric.h \
  net_except.h \
  inet_addr.h       inet_addr.cc \
  acceptor.h        acceptor.cc \
  reactor.h	    reactor.cc \
  connection.h      connection.cc \
  mt.h              mt.cc \
  ssl_lib.h         ssl_lib.cc \
  ssl_connection.h  ssl_connection.cc
