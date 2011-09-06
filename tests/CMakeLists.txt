find_package(Boost 1.41.0 COMPONENTS unit_test_framework program_options)

include_directories(${PROJECT_SOURCE_DIR})
add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(value-test test_value_usage.cc)
add_executable(parser-test parser2.cc)
set(CLIENT_COMMON_SRC client_common.h client_common.cc client_opts.h client_opts.cc)
add_executable(client-test ${CLIENT_COMMON_SRC} client.cc)
add_executable(client-stress-test ${CLIENT_COMMON_SRC} client_stress.cc)
add_executable(server-test test_server.cc server_config.h server_config.cc methods.h methods.cc)
add_executable(server-stop-test test_server_stop.cc)

foreach(t value-test parser-test client-test client-stress-test server-test server-stop-test)
	target_link_libraries(${t} iqxmlrpc ${Boost_LIBRARIES})
endforeach(t)
