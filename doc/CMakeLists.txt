find_program(DOXYGEN doxygen)
string(COMPARE EQUAL ${DOXYGEN} "DOXYGEN-NOTFOUND" DOXYGEN_NOTFOUND)

if (DOXYGEN_NOTFOUND)
	message("iqxmlrpc: Doxygen not found, skip docs generating")
	return()
endif (DOXYGEN_NOTFOUND)

configure_file(libiqxmlrpc.doxygen.in libiqxmlrpc.doxygen @ONLY)

add_custom_target(libiqxmlrpc.html ALL
	doxygen ${CMAKE_BINARY_DIR}/doc/libiqxmlrpc.doxygen
	DEPENDS ${CMAKE_BINARY_DIR}/doc/libiqxmlrpc.doxygen)

set(PROJ_DOC share/doc/libiqxmlrpc-${Libiqxmlrpc_VERSION})
install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/libiqxmlrpc.html DESTINATION ${PROJ_DOC})
install(FILES ../License ../NEWS ../README DESTINATION ${PROJ_DOC})

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES libiqxmlrpc.html)
