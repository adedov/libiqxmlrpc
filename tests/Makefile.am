EXTRA_DIST = data regression.sh
LDADD = ../libiqxmlrpc/libiqxmlrpc.la -lboost_unit_test_framework -lboost_program_options

check_PROGRAMS = \
  value-usage \
  server-test \
  client-test \
	client-stress-test \
  stop-test-server

value_usage_SOURCES = test_value_usage.cc

server_test_SOURCES = \
  test_server.cc \
  server_config.cc	server_config.h \
  server_classes.cc	server_classes.h \
  methods.cc				methods.h

CLIENT_COMMON_SOURCES = \
  client_common.cc	client_common.h \
	client_opts.cc		client_opts.h
	
client_test_SOURCES = \
	${CLIENT_COMMON_SOURCES} \
	client.cc

client_stress_test_SOURCES = \
	${CLIENT_COMMON_SOURCES} \
	thread_counter.cc	thread_counter.h \
  client_stress.cc

stop_test_server_SOURCES = \
  ${CLIENT_COMMON_SOURCES} \
  client.cc
