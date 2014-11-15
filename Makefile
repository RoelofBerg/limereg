APP = limereg

OBJDIR = obj
BINDIR = bin
INSTALLDIR = /usr/bin

SRCS := $(shell find . -name '*.cpp')
SRCDIRS := $(shell find . -name '*.cpp' -exec dirname {} \; | uniq)
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS := $(patsubst %.cpp,$(OBJDIR)/%.d,$(SRCS))
EXEPATH = $(BINDIR)/$(APP)

DEBUG = -g
INCLUDES = -I/usr/include/opencv -I./limereg -I./limereg/matlab -I./limereg/matlab/codegeneration
CFLAGS = $(DEBUG) -fopenmp $(INCLUDES) -c
#todo: add -Wall -pedantic
LDFLAGS = -fopenmp
LIBS = -lopencv_core -lopencv_highgui -lboost_program_options

DEPENDS = -MT $@ -MD -MP -MF $(subst .o,.d,$@)

SHELL = /bin/bash

.PHONY: all clean distclean

install: $(EXEPATH)
	sudo cp $(EXEPATH) $(INSTALLDIR)

uninstall:
	sudo rm -i $(INSTALLDIR)/$(APP)

all: buildrepo $(EXEPATH)

$(EXEPATH): $(OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CFLAGS) $(DEPENDS) $< -o $@

clean:
	$(RM) -r $(OBJDIR)
	$(RM) -r $(BINDIR)

distclean: clean
	$(RM) $(APP)

buildrepo:
	@$(call make-repo)

define make-repo
for dir in $(SRCDIRS); \
do \
mkdir -p $(OBJDIR)/$$dir; \
done
endef

ifneq "$(MAKECMDGOALS)" "distclean"
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEPS)
endif
endif
