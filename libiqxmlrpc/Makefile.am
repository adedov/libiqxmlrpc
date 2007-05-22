LDFLAGS += -version-info @LIBIQXMLRPC_VERSION@

noinst_LTLIBRARIES = libiqxmlrpc-general.la
libiqxmlrpc_general_la_LIBADD = ${LIBXMLPP_LIBS} ${OPENSSL_LIBS}

lib_LTLIBRARIES = libiqxmlrpc-client.la libiqxmlrpc-server.la libiqxmlrpc.la

libiqxmlrpc_client_la_LIBADD = libiqxmlrpc-general.la
libiqxmlrpc_server_la_LIBADD = libiqxmlrpc-general.la $(BOOST_THREAD_LIB)

libiqxmlrpc_la_SOURCES = libiqxmlrpc.h
libiqxmlrpc_la_LIBADD = libiqxmlrpc-general.la libiqxmlrpc-client.la libiqxmlrpc-server.la

EXTRA_DIST = version.h.in version.h 

noinst_HEADERS = \
	parser_specific.h \
	reactor_impl.h \
	reactor_poll_impl.h \
	reactor_select_impl.h \
	value_type_xml.h

pkginclude_HEADERS = \
	acceptor.h \
	api_export.h \
	builtins.h \
	client.h \
	connection.h \
	connector.h \
	conn_factory.h \
	dispatcher_manager.h \
	except.h \
	executor.h \
	http.h \
	http_client.h \
	http_server.h \
	https_client.h \
	https_server.h \
	inet_addr.h \
	libiqxmlrpc.h \
	lock.h \
	method.h \
	parser.h \
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
	utf_conv.h \
	util.h \
	value_type.h \
	value_type_visitor.h \
	value.h
#  xml_util.h

libiqxmlrpc_general_la_SOURCES = \
	acceptor.cc \
	connection.cc \
	dispatcher_manager.cc \
	except.cc \
	http.cc \
	inet_addr.cc \
	net_except.cc \
	parser.cc \
	parser_specific.cc \
	reactor_poll_impl.cc \
	reactor_select_impl.cc \
	request.cc \
	response.cc \
	socket.cc \
	ssl_lib.cc \
	ssl_connection.cc \
	utf_conv.cc \
	value_type.cc \
	value_type_visitor.cc \
	value_type_xml.cc \
	value.cc

libiqxmlrpc_client_la_SOURCES = \
	client.cc \
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
