find_program(DOXYGEN doxygen)
string(COMPARE EQUAL ${DOXYGEN} "DOXYGEN-NOTFOUND" DOXYGEN_NOTFOUND)

if (DOXYGEN_NOTFOUND)
	message("iqxmlrpc: Doxygen not found, skip docs generating")
	return()
endif (DOXYGEN_NOTFOUND)

configure_file(libiqxmlrpc.doxygen.in libiqxmlrpc.doxygen @ONLY)

add_custom_target(doxygen_docs ALL
	doxygen libiqxmlrpc.doxygen	
	DEPENDS libiqxmlrpc.doxygen)
