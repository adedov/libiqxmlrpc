ACLOCAL_AMFLAGS = -I ../m4
EXTRA_DIST = data
CXXFLAGS += -DBOOST_TEST_DYN_LINK
LDADD = ../libiqxmlrpc/libiqxmlrpc-client.la \
	../libiqxmlrpc/libiqxmlrpc-server.la \
	$(BOOST_THREAD_LIB) \
	$(BOOST_UNIT_TEST_FRAMEWORK_LIB) \
	$(BOOST_PROGRAM_OPTIONS_LIB)

check_PROGRAMS = \
	value-usage \
	server-test \
	server-stop-test \
	client-test \
	client-stress-test 

value_usage_SOURCES = test_value_usage.cc

server_test_SOURCES = \
	test_server.cc \
	server_config.cc  server_config.h \
	methods.cc        methods.h

server_stop_test_SOURCES = \
	test_server_stop.cc

CLIENT_COMMON_SOURCES = \
	client_common.cc  client_common.h \
	client_opts.cc    client_opts.h
	
client_test_SOURCES = \
	${CLIENT_COMMON_SOURCES} \
	client.cc

client_stress_test_SOURCES = \
	${CLIENT_COMMON_SOURCES} \
	client_stress.cc
