ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure config.h.in	config.log \
config.status stamp-h.in

SUBDIRS = lib exe doc tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = liblimereg.pc

.PHONY: beauty dist-hook

beauty:
	-for dir in lib exe tests; do cd $$dir; $(MAKE) $@; cd ..; done
