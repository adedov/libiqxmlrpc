AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = \
  autogen.sh \
  libiqxmlrpc.pc.in \
  libiqxmlrpc.pc \
  libiqnet.pc.in \
  libiqnet.pc

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
	cvs --fsf -P -S
