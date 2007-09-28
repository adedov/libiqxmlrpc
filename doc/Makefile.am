EXTRA_DIST = \
	libiqxmlrpc.doxygen \
	libiqxmlrpc.doxygen.in \
	users_guide/*.txt \
	users_guide/gen.sh

DOCDIR=${prefix}/share/doc/@PACKAGE@
MKDOC=X@MAKEDOCS@

all: libiqxmlrpc.html

libiqxmlrpc.html:
	[ ${MKDOC} == Xyes ] && doxygen libiqxmlrpc.doxygen || true

install-data-local: libiqxmlrpc.html
	[ ${MKDOC} == Xyes ] && mkdir -p ${DOCDIR}/libiqxmlrpc.html || true
	[ ${MKDOC} == Xyes ] && install -m 644 libiqxmlrpc.html/* ${DOCDIR}/libiqxmlrpc.html || true

uninstall-local:
	rm -rf ${DOCDIR}/libiqxmlrpc.html
	rmdir ${DOCDIR}

clean-local:
	if [ -d libiqxmlrpc.html ]; then rm -rf libiqxmlrpc.html; fi
