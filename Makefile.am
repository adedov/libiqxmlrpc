AUTOMAKE_OPTIONS = foreign

SUBDIRS = libiqxmlrpc tests doc
 
rpm:	dist
	cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	rpmbuild -ba $(PACKAGE).spec
