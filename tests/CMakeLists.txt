find_package(Boost 1.41.0 COMPONENTS unit_test_framework program_options thread)

include_directories(${OPENSSL_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${PROJECT_SOURCE_DIR})

if (NOT Boost_USE_STATIC_LIBS)
	add_definitions(-DBOOST_TEST_DYN_LINK)
endif (NOT Boost_USE_STATIC_LIBS)

macro (iqxmlrpc_test test_name)
	message("Add test ${test_name}")
	add_executable(${test_name} ${ARGN})
	target_link_libraries(${test_name} iqxmlrpc ${Boost_LIBRARIES} ${LIBXML2_LIBRARIES} crypto)
  link_directories(../libiqxmlrpc)
endmacro (iqxmlrpc_test)

iqxmlrpc_test(value-test test_value_usage.cc)
iqxmlrpc_test(server-test test_server.cc server_config.h server_config.cc methods.h methods.cc)
set(CLIENT_COMMON_SRC client_common.h client_common.cc client_opts.h client_opts.cc)
iqxmlrpc_test(client-test ${CLIENT_COMMON_SRC} client.cc)
iqxmlrpc_test(client-stress-test ${CLIENT_COMMON_SRC} client_stress.cc)

if (NOT WIN32)
	iqxmlrpc_test(parser-test parser2.cc)
endif (NOT WIN32)

# TODO: server-stop-test
