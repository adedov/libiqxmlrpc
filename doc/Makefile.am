EXTRA_DIST = \
  libiqxmlrpc.doxygen \
  libiqnet.doxygen \
  libiqxmlrpc.pages/*.dox \
  index.html

DOCDIR=${prefix}/share/doc/@PACKAGE@

doc:
	doxygen libiqnet.doxygen
	doxygen libiqxmlrpc.doxygen

install-data-local: doc
	${top_srcdir}/mkinstalldirs ${DOCDIR}
	${top_srcdir}/mkinstalldirs ${DOCDIR}/libiqnet.html
	${top_srcdir}/mkinstalldirs ${DOCDIR}/libiqxmlrpc.html
	install -m 644 index.html ${DOCDIR}
	install -m 644 libiqnet.html/* ${DOCDIR}/libiqnet.html
	install -m 644 libiqxmlrpc.html/* ${DOCDIR}/libiqxmlrpc.html

uninstall-local:
	rm -f ${DOCDIR}/index.html
	rm -rf ${DOCDIR}/libiqnet.html
	rm -rf ${DOCDIR}/libiqxmlrpc.html
	rmdir ${DOCDIR}

clean-local:
	if [ -d libiqnet.html ]; then rm -rf libiqnet.html; fi
	if [ -d libiqxmlrpc.html ]; then rm -rf libiqxmlrpc.html; fi
