MAINTAINERCLEANFILES = Makefile.in

#Build exe only if boost and opencv are present
if HAVE_BOOST
if HAVE_OPENCV

bin_PROGRAMS = limereg
limereg_SOURCES = CLogger.cpp CLogger.h CRegistrationController.cpp CRegistrationController.h limereg_common.h \
main.cpp stdafx.cpp stdafx.h targetver.h TimeMeasurement.h

limereg_CFLAGS = @CFLAGS@
limereg_CPPFLAGS = @CPPFLAGS@ -I. -I$(top_srcdir)/lib/export
limereg_LDFLAGS = @LDFLAGS@
limereg_LDADD = $(top_builddir)/lib/liblimereg.la

limereg_CFLAGS += @BOOST_CFLAGS@
limereg_CPPFLAGS += @BOOST_CPPFLAGS@
limereg_LDFLAGS += @BOOST_PROGRAM_OPTIONS_LDFLAGS@
limereg_LDADD += @BOOST_PROGRAM_OPTIONS_LIBS@

limereg_CFLAGS += @OPENCV_CFLAGS@
limereg_CPPFLAGS += @OPENCV_CFLAGS@
#limereg_LDFLAGS += @OPENCV_LDFLAGS@
limereg_LDADD += @OPENCV_LIBS@

#Manpages for the command-line utility
if HAVE_HELP2MAN

man1_MANS=../doc/limereg.1
CLEANFILES=../doc/limereg.1

../doc/limereg.1: $(bin_PROGRAMS)
	$(HELP2MAN) -n -o $@ ./$<

endif

endif
endif

.PHONY: beauty

beauty:
	-for file in $$(find . \( -name "*.c" -o -name "*.cpp" -o -name "*.h" \)); do indent -orig -i4 -ncdb $$file; done
	-rm -f `find . -name '*~'`
