APP = limereg
#Keep version in sync to limereg_common.h
MAJOR = 1
MINOR = 2.0

#Optimization (can be switched on/off here. Debug symbols will allways be generated as separate files in $(DBGDIR))
OLINK=-flto
OCOMP=-Ofast

OBJDIR = obj
BINDIR = bin
DBGDIR = dbg
LIBDIR = lib
INSTALLDIR = $(DESTDIR)/usr/bin
LIBINSTALLDIR = $(DESTDIR)/usr/lib
HDRINSTALLDIR = $(DESTDIR)/usr/include
DEVLIB = lib$(APP).so
SONAME = $(DEVLIB).$(MAJOR)
LIBNAME = $(SONAME).$(MINOR)
DBGEXT = .debug

SRCS := $(shell find src -name '*.cpp')
SRCDIRS := $(shell find src -name '*.cpp' -exec dirname {} \; | uniq)
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS := $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRCS))
LIBOBJS = $(OBJDIR)/$(LIBDIR)/lib$(APP).o
EXEPATH = $(BINDIR)/$(APP)
EXEDBGPATH = $(DBGDIR)/$(APP)$(DBGEXT)
LIBPATH = $(BINDIR)/$(LIBNAME)
LIBDBGPATH = $(DBGDIR)/$(LIBNAME)$(DBGEXT)
MANPAGE = obj/$(APP).1

DEBUG = 
INCLUDES = -I/usr/include/opencv -I./src -I./src/matlab -I./src/matlab/codegeneration
LIBFLAGS = -fpic 
COMMONFLAGS = -g -fopenmp $(OLINK)
#Note: g will be stripped off into separate files in the dbg folder later 
CFLAGS = $(DEBUG) $(LIBFLAGS) $(COMMONFLAGS) $(OCOMP)  $(INCLUDES) -c
#todo: add -Wall -pedantic
LDFLAGS = $(COMMONFLAGS)
LIBS = -lopencv_core -lopencv_highgui -lboost_program_options

DEPENDS = -MT $@ -MD -MP -MF $(subst .o,.d,$@)

SHELL = /bin/bash

.PHONY: all clean distclean

all: exe $(MANPAGE)

exe: buildrepo $(EXEPATH) $(EXEDBGPATH)

lib: buildlibrepo $(LIBPATH) $(LIBDBGPATH)

$(MANPAGE): $(EXEPATH)
	help2man --name="Lightweight Image Registration" $(EXEPATH) > $(MANPAGE)

$(LIBPATH): $(OBJS) $(LIBOBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) -shared $^ $(LIBS) -o $@ $(LIBFLAGS) -Wl,-soname,$(SONAME),--version-script=$(LIBDIR)/export.map

$(EXEPATH): $(OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CFLAGS) $(DEPENDS) $< -o $@

$(DBGDIR)/%$(DBGEXT): $(BINDIR)/%
	mkdir -p $(DBGDIR)
	objcopy --only-keep-debug $< $@
	strip --strip-debug --strip-unneeded $<
	objcopy --add-gnu-debuglink=$@ $<
	chmod -x $@

test: exe
	$(EXEPATH) --tfile testimg/T_4096.bmp --rfile testimg/R_4096.bmp --nogui

clean:
	$(RM) -r $(OBJDIR)
	$(RM) -r $(BINDIR)
	$(RM) -r $(DBGDIR)

distclean: clean
	$(RM) $(APP)

buildrepo:
	@$(call make-repo)

buildlibrepo: buildrepo
	@$(call make-lib-repo)

install: all
	mkdir -p $(INSTALLDIR)
	cp $(EXEPATH) $(INSTALLDIR)
	#manpage
	mkdir -p $(DESTDIR)/usr/share/man/man1/
	cp $(MANPAGE) $(DESTDIR)/usr/share/man/man1/

#Install library for users
libinstall: lib
	mkdir -p $(LIBINSTALLDIR)
	cp $(LIBPATH) $(LIBINSTALLDIR)
	ldconfig -n $(LIBINSTALLDIR)

#Install library for developers (also the header)
libinstall-dev: libinstall
	mkdir -p $(HDRINSTALLDIR)
	ln -fs $(LIBINSTALLDIR)/$(SONAME) $(LIBINSTALLDIR)/$(DEVLIB)
	cp $(LIBDIR)/$(APP).h $(HDRINSTALLDIR)

#Install library with debug symbols (near the lib to be automatically found)
libinstall-dbg: libinstall-dev 
	cp $(LIBDBGPATH) $(LIBINSTALLDIR)

uninstall:
	rm -i $(INSTALLDIR)/$(APP)
	rm -i $(DESTDIR)/usr/share/man/man1/$(APP).1

libuninstall:
	rm -i $(LIBINSTALLDIR)/$(LIBNAME)
	ldconfig -n $(LIBINSTALLDIR)

libuninstall-dev: libuninstall
	rm -i $(LIBINSTALLDIR)/$(DEVLIB)
	rm -i $(HDRINSTALLDIR)/$(APP).h

libuninstall-dbg: libuninstall-dev 
	rm -i $(LIBINSTALLDIR)/$(LIBNAME)$(DBGEXT)

define make-repo
for dir in $(SRCDIRS); \
do \
mkdir -p $(OBJDIR)/$$dir; \
done
endef

define make-lib-repo
mkdir -p $(OBJDIR)/$(LIBDIR)
endef

ifneq "$(MAKECMDGOALS)" "distclean"
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif
endif
