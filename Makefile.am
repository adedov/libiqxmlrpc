ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	m4 \
	License \
	ChangeLog \
	NEWS \
	README \
	libiqxmlrpc.pc.in \
	libiqxmlrpc.pc \
	libiqxmlrpc-server.pc.in \
	libiqxmlrpc-server.pc \
	libiqxmlrpc-client.pc.in \
	libiqxmlrpc-client.pc \
	libiqxmlrpc.spec.in \
	libiqxmlrpc.spec

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = \
	libiqxmlrpc.pc \
	libiqxmlrpc-server.pc \
	libiqxmlrpc-client.pc
  
SUBDIRS = \
	libiqxmlrpc \
	tests \
	doc \
	win32

rpm:	dist
	cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -ba $(PACKAGE).spec

clog:
	cvs2cl --fsf -P -S

mydist:	dist
	( cd doc; tar cfz ../libiqxmlrpc-doc-@VERSION@.tar.gz index.html libiqxmlrpc.html )
