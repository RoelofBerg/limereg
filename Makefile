APP = limereg
MAJOR = 0
MINOR = 1.0 

OBJDIR = obj
BINDIR = bin
LIBDIR = lib
INSTALLDIR = $(DESTDIR)/usr/bin
LIBINSTALLDIR = $(DESTDIR)/usr/lib
HDRINSTALLDIR = $(DESTDIR)/usr/include
DEVLIB = lib$(APP).so
SONAME = $(DEVLIB).$(MAJOR)
LIBNAME = $(SONAME).$(MINOR)

SRCS := $(shell find . -name '*.cpp')
SRCDIRS := $(shell find . -name '*.cpp' -exec dirname {} \; | uniq)
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS := $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRCS))
LIBOBJS = $(OBJDIR)/$(LIBDIR)/$(APP).co
EXEPATH = $(BINDIR)/$(APP)
LIBPATH = $(BINDIR)/$(LIBNAME)
MANPAGE = obj/$(APP).1

DEBUG = 
INCLUDES = -I/usr/include/opencv -I./src -I./src/matlab -I./src/matlab/codegeneration
LIBFLAGS = -fpic 
CFLAGS = $(DEBUG) $(LIBFLAGS) -Ofast -flto -fopenmp $(INCLUDES) -c
#todo: add -Wall -pedantic
LDFLAGS = -flto -fopenmp
LIBS = -lopencv_core -lopencv_highgui -lboost_program_options

DEPENDS = -MT $@ -MD -MP -MF $(subst .o,.d,$@)

SHELL = /bin/bash

.PHONY: all clean distclean

install: all
	mkdir -p $(INSTALLDIR)
	cp $(EXEPATH) $(INSTALLDIR)
	#manpage
	mkdir -p $(DESTDIR)/usr/share/man/man1/
	cp $(MANPAGE) $(DESTDIR)/usr/share/man/man1/

libinstall: lib
	mkdir -p $(LIBINSTALLDIR)
	cp $(LIBPATH) $(LIBINSTALLDIR)
	ldconfig -n $(LIBINSTALLDIR)

libinstall-dev: libinstall
	mkdir -p $(HDRINSTALLDIR)
	ln -s $(LIBINSTALLDIR)/$(SONAME) $(LIBINSTALLDIR)/$(DEVLIB)
	cp $(LIBDIR)/$(APP).h $(HDRINSTALLDIR)

uninstall:
	rm -i $(INSTALLDIR)/$(APP)
	rm -i $(DESTDIR)/usr/share/man/man1/$(APP).1

libuninstall:
	rm -i $(LIBINSTALLDIR)/$(LIBNAME)
	ldconfig -n $(LIBINSTALLDIR)

libuninstall-dev: libuninstall
	rm -i $(LIBINSTALLDIR)/$(DEVLIB)
	rm -i $(HDRINSTALLDIR)/$(APP).h

all: exe $(MANPAGE)

exe: buildrepo $(EXEPATH)

lib: buildlibrepo $(LIBPATH)

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

$(OBJDIR)/%.co: %.c
	$(CC) $(LIBFLAGS) -c $< -o $@

test: exe
	$(EXEPATH) --tfile testimg/T_4096.bmp --rfile testimg/R_4096.bmp --nogui

clean:
	$(RM) -r $(OBJDIR)
	$(RM) -r $(BINDIR)

distclean: clean
	$(RM) $(APP)

buildrepo:
	@$(call make-repo)

buildlibrepo: buildrepo
	@$(call make-lib-repo)

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
