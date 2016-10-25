dnl GNU Autotools configuration file of limereg
dnl License of limereg see file COPYING (BSD 3-Clause)
dnl Roelof Berg <rberg@berg-solutions.de>
dnl

dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.68)
AC_INIT([limereg],[1.4.1])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_SUBST([ac_aux_dir])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([configure.ac])
AM_CONFIG_HEADER([config.h])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
#AM_PROG_CC_C_O
#AC_PROG_CC_C99
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AM_MISSING_PROG([AUTOM4TE], [autom4te])
AC_PROG_HELP2MAN()

dnl Checks for libraries.
AC_LANG(C)
AC_LANG(C++)

AC_OPENMP
if test -n "${OPENMP_CXXFLAGS}"; then
  AC_SUBST(OPENMP_CFLAGS)
  AC_SUBST(OPENMP_CXXFLAGS)
  AC_SUBST(OPENMP_LIBS,"-lgomp")
  #non-openmp
else
  AC_MSG_WARN([*** openmp disabled. The application cannot be executed in parallel on several cpu cores, without openmp it will be single-threaded. This slows down the calculation on multicore systems. ***])
fi

HAVE_OPENCV=false
PKG_CHECK_MODULES(OPENCV, opencv >= 2.3.1, [HAVE_OPENCV=true], [true])
if test x$HAVE_OPENCV = xfalse; then
  AC_MSG_WARN([*** opencv >= 2.3.1 not found - Cannot build the command line tool, which relies on OpenCV. Only the library will be built, because the lib has no dependency to OpenCV. http://opencvlibrary.sourceforge.net ***])
  AC_DEFINE([HAVE_OPENCV],[0],[Define to 1 if you have OpenCV >= 2.3.1])
else
  OPENCV_CFLAGS="$OPENCV_CFLAGS -DOPENCV_PREFIX=`pkg-config opencv --variable=prefix`"
  #OPENCV_INCLUDE=pkg-config opencv --variable=includedir_old
  AC_DEFINE([HAVE_OPENCV],[1],[Define to 1 if you have OpenCV >= 2.3.1])
fi
AM_CONDITIONAL([HAVE_OPENCV], [test x$HAVE_OPENCV = xtrue])

#ToDo: Make Boost optional like OpenCV. (Boost_Require takes two arguments if found and if not ...)
HAVE_BOOST=false
AC_CHECK_HEADERS([boost/program_options.hpp], [HAVE_BOOST=true], [true])
if test x$HAVE_BOOST = xfalse; then
  AC_MSG_WARN([*** Boost-program-options not found - Cannot build the command line tool, which relies on Boost. Only the library will be built, because the lib has no dependency to Boost. http://www.boost.org ***])
else
  BOOST_REQUIRE
  BOOST_PROGRAM_OPTIONS()
fi
AM_CONDITIONAL([HAVE_BOOST], [test x$HAVE_BOOST = xtrue])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h stddef.h stdint.h stdlib.h string.h sys/time.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
#AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

dnl Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memset pow sqrt])

dnl Doxygen library documentation
DX_HTML_FEATURE(OFF)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(ON)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(liblimereg, doxygen.cfg)


AC_CONFIG_FILES([Makefile
                 lib/Makefile
                 exe/Makefile
                 doc/Makefile
                 tests/Makefile
                 tests/testimg/Makefile])

AC_OUTPUT(liblimereg.pc)
