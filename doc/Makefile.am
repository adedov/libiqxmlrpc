EXTRA_DIST = \
  libiqxmlrpc.doxygen \
  libiqxmlrpc.doxygen.in \
  libiqxmlrpc.pages/*.dox \
  index.html \
  README.win32

DOCDIR=${prefix}/share/doc/@PACKAGE@

all: libiqxmlrpc.html index.html

libiqxmlrpc.html:
	doxygen libiqxmlrpc.doxygen

install-data-local: libiqxmlrpc.html index.html
	mkdir -p ${DOCDIR}/libiqxmlrpc.html
	install -m 644 index.html ${DOCDIR}
	install -m 644 libiqxmlrpc.html/* ${DOCDIR}/libiqxmlrpc.html

uninstall-local:
	rm -f ${DOCDIR}/index.html
	rm -rf ${DOCDIR}/libiqxmlrpc.html
	rmdir ${DOCDIR}

clean-local:
	if [ -d libiqxmlrpc.html ]; then rm -rf libiqxmlrpc.html; fi
