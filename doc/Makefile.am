EXTRA_DIST = \
  libiqxmlrpc.doxygen \
  libiqxmlrpc.doxygen.in \
  libiqxmlrpc.pages/*.dox \
  index.html

DOCDIR=${prefix}/share/doc/@PACKAGE@

doc:
	doxygen libiqxmlrpc.doxygen

install-data-local: doc
	${top_srcdir}/mkinstalldirs ${DOCDIR}
	${top_srcdir}/mkinstalldirs ${DOCDIR}/libiqxmlrpc.html
	install -m 644 index.html ${DOCDIR}
	install -m 644 libiqxmlrpc.html/* ${DOCDIR}/libiqxmlrpc.html

uninstall-local:
	rm -f ${DOCDIR}/index.html
	rm -rf ${DOCDIR}/libiqxmlrpc.html
	rmdir ${DOCDIR}

clean-local:
	if [ -d libiqxmlrpc.html ]; then rm -rf libiqxmlrpc.html; fi
