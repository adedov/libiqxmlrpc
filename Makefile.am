AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = \
  License \
  ChangeLog \
  README \
  autogen.sh \
  libiqxmlrpc.pc.in \
  libiqxmlrpc.pc \
  libiqnet.pc.in \
  libiqnet.pc \
  libiqxmlrpc.spec.in \
  libiqxmlrpc.spec

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = \
  libiqxmlrpc.pc \
  libiqnet.pc

SUBDIRS = \
  libiqnet \
  libiqxmlrpc \
  tests.libiqnet \
  tests \
  doc
 
rpm:	dist
	cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -ba $(PACKAGE).spec

clog:
	cvs2cl --fsf -P -S
