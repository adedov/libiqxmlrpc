cmake_minimum_required(VERSION 2.8)
project(Libiqxmlrpc)
set(Libiqxmlrpc_VERSION 0.10.1)

add_subdirectory(libiqxmlrpc)

option(build_tests "Build tests?" OFF)
if (build_tests)
	add_subdirectory(tests)
endif (build_tests)

option(build_docs "Build doxygen docs?" ON)
if (build_docs)
	add_subdirectory(doc)
endif (build_docs)

add_custom_target(dist
	COMMAND bzr export --root=libiqxmlrpc-${Libiqxmlrpc_VERSION} libiqxmlrpc-${Libiqxmlrpc_VERSION}.tar.bz2
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

## CPack
set(CPACK_PACKAGE_NAME libiqxmlrpc)
set(CPACK_PACKAGE_VERSION ${Libiqxmlrpc_VERSION})
set(CPACK_PACKAGE_VENDOR "Anton Dedov")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple object-oriented XML-RPC client-server solution for C++.")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_HOME_DIRECTORY}/Description.txt)
# rpm
set(CPACK_RPM_PACKAGE_LICENSE "BSD")
set(CPACK_RPM_PACKAGE_GROUP "System Environment/Libraries")
include(CPack)
