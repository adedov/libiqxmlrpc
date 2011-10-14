LDFLAGS += -version-info @LIBIQXMLRPC_VERSION@

noinst_LTLIBRARIES = libiqxmlrpc-general.la
libiqxmlrpc_general_la_LIBADD = ${LIBXML2_LIBS} ${OPENSSL_LIBS}

lib_LTLIBRARIES = libiqxmlrpc-client.la libiqxmlrpc-server.la libiqxmlrpc.la

libiqxmlrpc_client_la_LIBADD = libiqxmlrpc-general.la
libiqxmlrpc_server_la_LIBADD = libiqxmlrpc-general.la $(BOOST_THREAD_LIB)

libiqxmlrpc_la_SOURCES = libiqxmlrpc.h
libiqxmlrpc_la_LIBADD = libiqxmlrpc-general.la libiqxmlrpc-client.la libiqxmlrpc-server.la

EXTRA_DIST = version.h.in version.h 

noinst_HEADERS = \
  parser2.h \
	value_parser.h \
	request_parser.h \
	response_parser.h \
	reactor_impl.h \
	reactor_poll_impl.h \
	reactor_select_impl.h \
	value_type_xml.h \
	xml_builder.h

pkginclude_HEADERS = \
	acceptor.h \
	api_export.h \
	auth_plugin.h \
	builtins.h \
	client.h \
	client_conn.h \
	client_opts.h \
	connection.h \
	connector.h \
	conn_factory.h \
	dispatcher_manager.h \
	except.h \
	executor.h \
	firewall.h \
	http.h \
	http_errors.h \
	http_client.h \
	http_server.h \
	https_client.h \
	https_server.h \
	inet_addr.h \
	libiqxmlrpc.h \
	lock.h \
	method.h \
	net_except.h \
	reactor.h \
	reactor_interrupter.h \
	request.h \
	response.h \
	server_conn.h \
	server.h \
	socket.h \
	ssl_lib.h \
	ssl_connection.h \
	sysinc.h \
	util.h \
	value_type.h \
	value_type_visitor.h \
	value.h

libiqxmlrpc_general_la_SOURCES = \
	acceptor.cc \
	connection.cc \
	dispatcher_manager.cc \
	http.cc \
	inet_addr.cc \
	net_except.cc \
	parser2.cc \
	reactor_poll_impl.cc \
	reactor_select_impl.cc \
	request.cc \
	request_parser.cc \
	response.cc \
	response_parser.cc \
	socket.cc \
	ssl_lib.cc \
	ssl_connection.cc \
	value_type.cc \
	value_type_visitor.cc \
	value_type_xml.cc \
	value.cc \
	value_parser.cc \
	xml_builder.cc

libiqxmlrpc_client_la_SOURCES = \
	connector.cc \
	client.cc \
	client_conn.cc \
	http_client.cc \
	https_client.cc

libiqxmlrpc_server_la_SOURCES = \
	auth_plugin.cc \
	builtins.cc \
	executor.cc \
	http_server.cc \
	https_server.cc \
	method.cc \
	reactor_interrupter.cc \
	server.cc \
	server_conn.cc

clean-local:
	rm -f *.da  
