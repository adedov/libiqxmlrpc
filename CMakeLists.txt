cmake_minimum_required(VERSION 2.8)
project(Libiqxmlrpc)
set(Libiqxmlrpc_VERSION 0.13.7)

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
	COMMAND git log > ChangeLog)

add_custom_target(dist
	COMMAND git archive --prefix=libiqxmlrpc-${Libiqxmlrpc_VERSION}/ --format=tar HEAD | bzip2 > libiqxmlrpc-${Libiqxmlrpc_VERSION}.tar.bz2
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

include(LibiqxmlrpcCPack.txt)
