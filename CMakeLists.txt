cmake_minimum_required(VERSION 2.8)
project(Libiqxmlrpc)
set(Libiqxmlrpc_VERSION 0.10.1)

add_subdirectory(libiqxmlrpc)

option(build_tests "Build tests?" OFF)
if (build_tests)
	add_subdirectory(tests)
endif (build_tests)

set(CPACK_PACKAGE_NAME libiqxmlrpc)
set(CPACK_PACKAGE_VERSION ${Libiqxmlrpc_VERSION})
include(CPack)
