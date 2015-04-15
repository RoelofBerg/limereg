MAINTAINERCLEANFILES = Makefile.in

lib_LTLIBRARIES = liblimereg.la

liblimereg_la_SOURCES = CMatlabArray.cpp CMatlabArray.h CMatlabArrayTypedefs.h CRegistrator.cpp CRegistrator.h \
export/limereg.cpp matlab/matlab_c_ssdRigid2D.cpp matlab/matlab_c_ssdRigid2D.h matlab/pseudo_stdafx.h \
limereg_common.h limereg_exceptions.h stdafx.cpp stdafx.h \
matlab/codegeneration/all.cpp matlab/codegeneration/all.h\
matlab/codegeneration/calcMarginAddition.cpp matlab/codegeneration/calcMarginAddition.h\
matlab/codegeneration/diffimg.cpp matlab/codegeneration/diffimg.h\
matlab/codegeneration/gaussnewton.cpp matlab/codegeneration/gaussnewton.h\
matlab/codegeneration/gen_example_data.cpp matlab/codegeneration/gen_example_data.h\
matlab/codegeneration/generatePyramidPC.cpp matlab/codegeneration/generatePyramidPC.h\
matlab/codegeneration/jacobian.cpp matlab/codegeneration/jacobian.h\
matlab/codegeneration/limereg_emxAPI.cpp matlab/codegeneration/limereg_emxAPI.h\
matlab/codegeneration/limereg_emxutil.cpp matlab/codegeneration/limereg_emxutil.h\
matlab/codegeneration/limereg_initialize.cpp matlab/codegeneration/limereg_initialize.h\
matlab/codegeneration/limereg_rtwutil.cpp matlab/codegeneration/limereg_rtwutil.h\
matlab/codegeneration/limereg_terminate.cpp matlab/codegeneration/limereg_terminate.h\
matlab/codegeneration/limereg_types.h\
matlab/codegeneration/mldivide.cpp matlab/codegeneration/mldivide.h\
matlab/codegeneration/mod.cpp matlab/codegeneration/mod.h\
matlab/codegeneration/mpower.cpp matlab/codegeneration/mpower.h\
matlab/codegeneration/norm.cpp matlab/codegeneration/norm.h\
matlab/codegeneration/rt_nonfinite.cpp matlab/codegeneration/rt_nonfinite.h\
matlab/codegeneration/rtwtypes.h\
matlab/codegeneration/rtGetInf.cpp matlab/codegeneration/rtGetInf.h\
matlab/codegeneration/rtGetNaN.cpp matlab/codegeneration/rtGetNaN.h\
matlab/codegeneration/shrinkImageDSP.cpp matlab/codegeneration/shrinkImageDSP.h\
matlab/codegeneration/ssd.cpp matlab/codegeneration/ssd.h\
matlab/codegeneration/transform.cpp matlab/codegeneration/transform.h

liblimereg_la_CPPFLAGS = @CPPFLAGS@
liblimereg_la_CFLAGS = @CFLAGS@
liblimereg_la_LDFLAGS = -version-info 1:0:0 -export-symbols-regex '^Limereg_'
#liblimereg_la_LIBADD = -lm
liblimereg_la_LIBADD =

liblimereg_la_CPPFLAGS += @OPENMP_CXXFLAGS@
liblimereg_la_CFLAGS += @OPENMP_CFLAGS@
#liblimereg_la_LDFLAGS += @OPENMP_LDFLAGS@
liblimereg_la_LIBADD += @OPENMP_LIBS@

include_HEADERS = export/limereg.h

.PHONY: beauty

beauty:
	-for file in $$(find . \( -name "*.c" -o -name "*.cpp" -o -name "*.h" \)); do indent -orig -i4 -ncdb $$file; done
	-rm -f `find . -name '*~'`
