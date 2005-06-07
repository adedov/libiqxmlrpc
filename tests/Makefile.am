EXTRA_DIST = data regression.sh
LDADD = ../libiqxmlrpc/libiqxmlrpc.la -lboost_unit_test_framework

check_PROGRAMS = \
  value-usage \
  server-test \
  client-test \
  stop-test-server

value_usage_SOURCES = test_value_usage.cc

server_test_SOURCES = \
  test_server.cc \
  server_config.cc	server_config.h \
  server_classes.cc	server_classes.h \
  methods.cc		methods.h

client_test_SOURCES = client.cc

stop_test_server_SOURCES = client.cc
