EXTRA_DIST = data
LDADD = ../libiqxmlrpc/libiqxmlrpc.la

check_PROGRAMS = \
  parse-value \
  new-value \
  parse-request \
  parse-response \
  server-test1 \
  server-test2 \
  parse-http \
  new-http-resp \
  new-http-req

parse_value_SOURCES = parse_value.cc

new_value_SOURCES = new_value.cc

parse_request_SOURCES = parse_request.cc

parse_response_SOURCES = parse_response.cc

server_test1_SOURCES = server_test1.cc

server_test2_SOURCES = server_test2.cc
server_test2_LDADD = ../libiqnet/libiqnet.la ${LDADD}

parse_http_SOURCES = parse_http.cc

new_http_resp_SOURCES = new_http_resp.cc

new_http_req_SOURCES = new_http_req.cc