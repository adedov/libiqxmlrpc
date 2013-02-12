cmake_minimum_required(VERSION 2.8)
project(Libiqxmlrpc)
set(Libiqxmlrpc_VERSION 0.12.1)

add_subdirectory(libiqxmlrpc)

option(build_tests "Build tests?" OFF)
if (build_tests)
	add_subdirectory(tests)
endif (build_tests)

option(build_docs "Build doxygen docs?" ON)
if (build_docs)
	add_subdirectory(doc)
endif (build_docs)

add_custom_target(clog
	COMMAND bzr log --short > ChangeLog)

add_custom_target(dist
	COMMAND bzr export --root=libiqxmlrpc-${Libiqxmlrpc_VERSION} libiqxmlrpc-${Libiqxmlrpc_VERSION}.tar.bz2
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

include(LibiqxmlrpcCPack.txt)
