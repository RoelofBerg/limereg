#Prerequisites:
#
# make install-dev for https://github.com/RoelofBerg/liblimereg
# sudo apt-get install build-essentials libcv-dev libhighgui-dev libopencv-dev libboost-all-dev help2man
#
#Make targets:
#
#make all: Build the executable 'limereg' (shell application for image registration) and a man-page
#make exe: Build only 'limereg' without the man-page
#make install: Install 'limereg' to /usr/bin, so one call call it from everywhere. Also installs the man-page.
#make uninstall: Uninstall 'limereg' from /usr/bin
#make test: Use 'limereg' to register the images in the folder testimg. Expected (about): w = [5.04891 deg, 6.0895, 126.901]
#
#make clean: Remove the build output (executable) and all intermediate files (object files)
#


APP = limereg
#Keep version in sync to limereg_common.h
MAJOR = 1
MINOR = 2.0

#Optimization (can be switched on/off here, this has not so much impact as the same setting in liblimereg)
OLINK=-flto
OCOMP=-Ofast

SRCDIR = src
OBJDIR = obj
BINDIR = bin
DBGDIR = dbg
INSTALLDIR = $(DESTDIR)/usr/bin
MANSECT = 1
MANPATH = /usr/share/man/man$(MANSECT)/

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
SRCDIRS := $(shell find $(SRCDIR) -name '*.cpp' -exec dirname {} \; | uniq)
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS := $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRCS))
EXEPATH = $(BINDIR)/$(APP)
MANPAGE = obj/$(APP).$(MANSECT)

DEBUG = 
INCLUDES = -I/usr/include/opencv -I./src
COMMONFLAGS = $(OLINK)
#Note: g will be stripped off into separate files in the dbg folder later 
CFLAGS = $(DEBUG) $(COMMONFLAGS) $(OCOMP) $(INCLUDES) -c
#todo: add -Wall -pedantic
LDFLAGS = $(COMMONFLAGS)
LIBS = -l$(APP) -lopencv_core -lopencv_highgui -lboost_program_options

DEPENDS = -MT $@ -MD -MP -MF $(subst .o,.d,$@)

SHELL = /bin/bash

.PHONY: all clean distclean exe man

all: exe

exe: buildrepo $(EXEPATH)

man: $(MANPAGE)

$(MANPAGE): $(EXEPATH)
	help2man --name="Lightweight Image Registration" $(EXEPATH) > $(MANPAGE)

$(EXEPATH): $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@
 
$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CFLAGS) $(DEPENDS) $< -o $@

test: exe
	$(EXEPATH) --tfile testimg/T_4096.bmp --rfile testimg/R_4096.bmp --nogui | tee /dev/tty | grep -G 'w = \[5.* deg, 6.*, 126.*\]'

clean:
	$(RM) -r $(OBJDIR)
	$(RM) -r $(BINDIR)

buildrepo:
	@$(call make-repo)

install: all man
	mkdir -p $(INSTALLDIR)
	cp $(EXEPATH) $(INSTALLDIR)
	#manpage
	mkdir -p $(DESTDIR)$(MANPATH)
	cp $(MANPAGE) $(DESTDIR)$(MANPATH)

uninstall:
	rm -i $(INSTALLDIR)/$(APP)
	rm -i $(DESTDIR)$(MANPATH)$(APP).$(MANSECT)

define make-repo
for dir in $(SRCDIRS); \
do \
mkdir -p $(OBJDIR)/$$dir; \
done
mkdir -p $(BINDIR)
endef

ifneq "$(MAKECMDGOALS)" "distclean"
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif
endif
