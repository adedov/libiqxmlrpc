AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = \
  License \
  ChangeLog \
  NEWS \
  README \
  autogen.sh \
  libiqxmlrpc.pc.in \
  libiqxmlrpc.pc \
  libiqxmlrpc.spec.in \
  libiqxmlrpc.spec

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = libiqxmlrpc.pc

SUBDIRS = \
  libiqxmlrpc \
  tests \
  doc \
  win32

#  libiqnet
 
rpm:	dist
	cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -ba $(PACKAGE).spec

clog:
	cvs2cl --fsf -P -S

mydist:	dist doc
	( cd doc; make doc )
	( cd doc; tar cfz ../libiqxmlrpc-doc-@VERSION@.tar.gz index.html libiqxmlrpc.html )
