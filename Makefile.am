AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = autogen.sh

SUBDIRS = \
  libiqnet \
  libiqxmlrpc \
  tests.libiqnet \
  tests \
  doc
 
rpm:	dist
	cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -ba $(PACKAGE).spec
