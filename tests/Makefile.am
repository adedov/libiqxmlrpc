EXTRA_DIST = data
#LDADD = ../libiqxmlrpc/libiqxmlrpc.la

check_PROGRAMS = \
  parse-response \
  parse-request \
  parse-value \
  base64-encode \
  base64-decode \
  value-usage \
  http-server \
  http-server-thr \
  https-server \
  http-client \
  http-client-i \
  https-client \
  https-client-i

parse_value_SOURCES = parse_value.cc
parse_value_LDADD = ../libiqxmlrpc/libiqxmlrpc.la

base64_encode_SOURCES = base64.cc
base64_encode_LDADD = ../libiqxmlrpc/libiqxmlrpc.la

base64_decode_SOURCES = base64.cc
base64_decode_LDADD = ../libiqxmlrpc/libiqxmlrpc.la

value_usage_SOURCES = value_usage.cc
value_usage_LDADD = ../libiqxmlrpc/libiqxmlrpc.la

parse_request_SOURCES = parse_request.cc
parse_request_LDADD = ../libiqxmlrpc/libiqxmlrpc.la

parse_response_SOURCES = parse_response.cc
parse_response_LDADD = ../libiqxmlrpc/libiqxmlrpc.la

http_server_SOURCES = server_general.h http_server.cc
http_server_LDADD = ../libiqxmlrpc/libiqxmlrpc-server.la

http_server_thr_SOURCES = server_general.h http_server_thr.cc
http_server_thr_LDADD = ../libiqxmlrpc/libiqxmlrpc-server.la

https_server_SOURCES = server_general.h https_server.cc
https_server_LDADD = ../libiqxmlrpc/libiqxmlrpc-server.la

http_client_SOURCES = client_general.h http_client.cc
http_client_LDADD = ../libiqxmlrpc/libiqxmlrpc-client.la

http_client_i_SOURCES = client_general.h http_client_i.cc
http_client_i_LDADD = ../libiqxmlrpc/libiqxmlrpc-client.la

https_client_SOURCES = client_general.h https_client.cc
https_client_LDADD = ../libiqxmlrpc/libiqxmlrpc-client.la

https_client_i_SOURCES = client_general.h https_client_i.cc
https_client_i_LDADD = ../libiqxmlrpc/libiqxmlrpc-client.la
