EXTRA_DIST = data
LDADD = ../libiqxmlrpc/libiqxmlrpc.la

check_PROGRAMS = \
  parse-response \
  parse-request \
  parse-value \
  value-usage \
  http-server \
  http-server-thr \
  https-server \
  http-client \
  http-client-i \
  https-client \
  https-client-i

parse_value_SOURCES = parse_value.cc

value_usage_SOURCES = value_usage.cc

parse_request_SOURCES = parse_request.cc

parse_response_SOURCES = parse_response.cc

http_server_SOURCES = server_general.h http_server.cc

http_server_thr_SOURCES = server_general.h http_server_thr.cc

https_server_SOURCES = server_general.h https_server.cc

http_client_SOURCES = client_general.h http_client.cc

http_client_i_SOURCES = client_general.h http_client_i.cc

https_client_SOURCES = client_general.h https_client.cc

https_client_i_SOURCES = client_general.h https_client_i.cc
