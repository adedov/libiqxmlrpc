EXTRA_DIST = \
  libiqxmlrpc.doxygen \
  libiqxmlrpc.doxygen.in \
  libiqxmlrpc.pages/*.dox \
  index.html \
  README.win32

DOCDIR=${prefix}/share/doc/@PACKAGE@
MKDOC=X@MAKEDOCS@

all: libiqxmlrpc.html index.html

libiqxmlrpc.html:
	[ ${MKDOC} == Xyes ] && doxygen libiqxmlrpc.doxygen || true

install-data-local: libiqxmlrpc.html index.html
	[ ${MKDOC} == Xyes ] && mkdir -p ${DOCDIR}/libiqxmlrpc.html || true
	[ ${MKDOC} == Xyes ] && install -m 644 index.html ${DOCDIR} || true
	[ ${MKDOC} == Xyes ] && install -m 644 libiqxmlrpc.html/* ${DOCDIR}/libiqxmlrpc.html || true

uninstall-local:
	rm -f ${DOCDIR}/index.html
	rm -rf ${DOCDIR}/libiqxmlrpc.html
	rmdir ${DOCDIR}

clean-local:
	if [ -d libiqxmlrpc.html ]; then rm -rf libiqxmlrpc.html; fi
