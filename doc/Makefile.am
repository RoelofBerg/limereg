MAINTAINERCLEANFILES = Makefile.in

include aminclude.am
	
#Manpages for the library
if DX_COND_man

EXTRA_DIST = \
	doxygen.cfg

clean-local:
	sudo rm -rf @DX_DOCDIR@

# ToDo: Why does man3_MANS not work ? Better generate the manpages during source-tarball-compilation and don't
# ship them as part of the tarball. Reason: Parts of the file might be dependent (e.g. unavailable) on the target
# system. 
dist_man3_MANS = @DX_DOCDIR@/man/man3/liblimereg.3 @DX_DOCDIR@/man/man3/limereg.h.3 

$(dist_man3_MANS): doxygen-doc

endif
