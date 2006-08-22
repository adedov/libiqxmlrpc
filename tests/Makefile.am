ACLOCAL_AMFLAGS = -I ../m4
EXTRA_DIST = data regression.sh
CXXFLAGS = @CXXFLAGS@ -pthread
LDADD = ../libiqxmlrpc/libiqxmlrpc-client.la \
	../libiqxmlrpc/libiqxmlrpc-server.la \
	$(BOOST_THREAD_LIB) \
	$(BOOST_UNIT_TEST_FRAMEWORK) \
	$(BOOST_PROGRAM_OPTIONS)

check_PROGRAMS = \
	value-usage \
	server-test \
	client-test \
	client-stress-test 

value_usage_SOURCES = test_value_usage.cc

server_test_SOURCES = \
	test_server.cc \
	server_config.cc  server_config.h \
	methods.cc        methods.h

CLIENT_COMMON_SOURCES = \
	client_common.cc  client_common.h \
	client_opts.cc    client_opts.h
	
client_test_SOURCES = \
	${CLIENT_COMMON_SOURCES} \
	client.cc

client_stress_test_SOURCES = \
	${CLIENT_COMMON_SOURCES} \
	client_stress.cc

client_stress_test_LDADD = ${LDADD} -lboost_thread
