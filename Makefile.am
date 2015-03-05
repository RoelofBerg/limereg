ACLOCAL_AMFLAGS = -I m4

AUX_DIST = $(ac_aux_dir)/compile $(ac_aux_dir)/config.guess	$(ac_aux_dir)/config.sub \
$(ac_aux_dir)/depcomp $(ac_aux_dir)/install-sh $(ac_aux_dir)/ltmain.sh \
$(ac_aux_dir)/missing

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure config.h.in	config.log \
config.status stamp-h.in $(AUX_DIST)

SUBDIRS = lib exe doc tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = liblimereg.pc

.PHONY: beauty dist-hook

beauty:
	-for dir in lib exe tests; do cd $$dir; $(MAKE) $@; cd ..; done

dist-hook:
	for file in $(AUX_DIST); do \
	  cp $$file $(distdir)/$$file; \
	done
	cp doc/doxygen.cfg $(distdir)/doc
