EXTRA_DIST = data
LDADD = ../libiqxmlrpc/libiqxmlrpc.la

check_PROGRAMS = \
  parse-value \
  new-value \
  parse-request \
  parse-response \
  server-test1 \
  server-test2 \
  server-test3 \
  server-test4 \
  client-test1

parse_value_SOURCES = parse_value.cc

new_value_SOURCES = new_value.cc

parse_request_SOURCES = parse_request.cc

parse_response_SOURCES = parse_response.cc

server_test1_SOURCES = server_test1.cc

server_test2_SOURCES = server_test2.cc
server_test2_LDADD = ../libiqnet/libiqnet.la ${LDADD}

server_test3_SOURCES = server_test3.cc
server_test3_LDADD = ../libiqnet/libiqnet.la ${LDADD}

server_test4_SOURCES = server_test4.cc
server_test4_LDADD = ../libiqxmlrpc/libiqxmlrpc-net.la

client_test1_SOURCES = client_test1.cc
client_test1_LDADD = ../libiqxmlrpc/libiqxmlrpc-net.la
