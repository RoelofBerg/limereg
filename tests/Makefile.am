## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/lib/export

SUBDIRS = testimg

include_HEADERS = test_common.h

noinst_PROGRAMS = test_getVersion test_findAlignment  test_searchSubimage
if HAVE_BOOST
if HAVE_OPENCV
noinst_PROGRAMS += test_commandLineTool
endif
endif

test_getVersion_LDFLAGS = @LDFLAGS@
test_getVersion_LDADD = $(top_builddir)/lib/liblimereg.la
test_findAlignment_LDFLAGS = @LDFLAGS@
test_findAlignment_LDADD = $(top_builddir)/lib/liblimereg.la 
test_searchSubimage_LDFLAGS = @LDFLAGS@
test_searchSubimage_LDADD = $(top_builddir)/lib/liblimereg.la

TESTS = test_getVersion test_findAlignment test_searchSubimage
if HAVE_BOOST
if HAVE_OPENCV
TESTS += test_commandLineTool
endif
endif

.PHONY: beauty

beauty:
	-for file in $$(find . \( -name "*.c" -o -name "*.cpp" -o -name "*.h" \)); do indent -orig -i4 -ncdb $$file; done
	-rm -f `find . -name '*~'`
