APP = limereg
MAJOR = 0
MINOR = 1.0 

OBJDIR = obj
BINDIR = bin
LIBDIR = lib
INSTALLDIR = /usr/bin
LIBINSTALLDIR = /usr/lib
SONAME = lib$(APP).so.$(MAJOR)
LIBNAME = $(SONAME).$(MINOR)

SRCS := $(shell find . -name '*.cpp')
SRCDIRS := $(shell find . -name '*.cpp' -exec dirname {} \; | uniq)
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS := $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRCS))
LIBOBJS = $(OBJDIR)/$(LIBDIR)/$(APP).co
EXEPATH = $(BINDIR)/$(APP)
LIBPATH = $(BINDIR)/$(LIBNAME)

DEBUG = 
INCLUDES = -I/usr/include/opencv -I./limereg -I./limereg/matlab -I./limereg/matlab/codegeneration
LIBFLAGS = -fpic 
CFLAGS = $(DEBUG) $(LIBFLAGS) -Ofast -flto -fopenmp $(INCLUDES) -c
#todo: add -Wall -pedantic
LDFLAGS = -flto -fopenmp
LIBS = -lopencv_core -lopencv_highgui -lboost_program_options

DEPENDS = -MT $@ -MD -MP -MF $(subst .o,.d,$@)

SHELL = /bin/bash

.PHONY: all clean distclean

install: all
	sudo cp $(EXEPATH) $(INSTALLDIR)

libinstall: lib
	sudo cp $(LIBPATH) $(LIBINSTALLDIR)
	sudo ldconfig -n $(LIBINSTALLDIR)

uninstall:
	sudo rm -i $(INSTALLDIR)/$(APP)

libuninstall:
	sudo rm -i $(LIBINSTALLDIR)/$(LIBNAME)
	sudo ldconfig -n $(LIBINSTALLDIR)

all: exe

exe: buildrepo $(EXEPATH)

lib: buildlibrepo $(LIBPATH)

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
