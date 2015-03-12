## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/lib/export

SUBDIRS = testimg

include_HEADERS = test_common.h

noinst_PROGRAMS = test_liblimereg
if HAVE_BOOST
if HAVE_OPENCV
noinst_PROGRAMS += test_limereg
endif
endif

test_liblimereg_LDFLAGS = @LDFLAGS@
test_liblimereg_LDADD = $(top_builddir)/lib/liblimereg.la

TESTS = test_liblimereg
if HAVE_BOOST
if HAVE_OPENCV
TESTS += test_limereg
endif
endif

.PHONY: beauty

beauty:
	-for file in $$(find . \( -name "*.c" -o -name "*.cpp" -o -name "*.h" \)); do indent -orig -i4 -ncdb $$file; done
	-rm -f `find . -name '*~'`
