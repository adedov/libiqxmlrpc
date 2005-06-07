EXTRA_DIST = data regression.sh
#LDADD = ../libiqxmlrpc/libiqxmlrpc.la

check_PROGRAMS = \
  value-usage \
  server-test \
  client-test \
  stop-test-server

value_usage_SOURCES = test_value_usage.cc
value_usage_LDADD = ../libiqxmlrpc/libiqxmlrpc.la -lboost_unit_test_framework

server_test_SOURCES = \
  test_server.cc \
  server_config.cc	server_config.h \
  server_classes.cc	server_classes.h \
  methods.cc		methods.h

server_test_LDADD = ../libiqxmlrpc/libiqxmlrpc.la -lboost_unit_test_framework

client_test_SOURCES = client.cc
client_test_LDADD = ../libiqxmlrpc/libiqxmlrpc.la -lboost_unit_test_framework

stop_test_server_SOURCES = client.cc
stop_test_server_LDADD = ../libiqxmlrpc/libiqxmlrpc.la -lboost_unit_test_framework
