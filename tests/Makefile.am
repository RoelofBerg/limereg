## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/lib/export

noinst_PROGRAMS = test_liblimereg

test_liblimereg_LDFLAGS = @LDFLAGS@
test_liblimereg_LDADD = $(top_builddir)/lib/liblimereg.la

TESTS = test_liblimereg

.PHONY: beauty

beauty:
	-for file in $$(find . \( -name "*.c" -o -name "*.cpp" -o -name "*.h" \)); do indent -orig -i4 -ncdb $$file; done
	-rm -f `find . -name '*~'`
