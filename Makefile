##============================================================================
##
##   SSSS    tt          lll  lll
##  SS  SS   tt           ll   ll
##  SS     tttttt  eeee   ll   ll   aaaa
##   SSSS    tt   ee  ee  ll   ll      aa
##      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
##  SS  SS   tt   ee      ll   ll  aa  aa
##   SSSS     ttt  eeeee llll llll  aaaaa
##
## Copyright (c) 1995-2015 by Bradford W. Mott, Stephen Anthony
## and the Stella Team
##
## See the file "License.txt" for information on usage and redistribution of
## this file, and for a DISCLAIMER OF ALL WARRANTIES.
##
## $Id: Makefile,v 1.37 2009-01-11 21:31:21 stephena Exp $
##
##   Based on code from ScummVM - Scumm Interpreter
##   Copyright (C) 2002-2004 The ScummVM project
##============================================================================

#######################################################################
# Default compilation parameters. Normally don't edit these           #
#######################################################################

srcdir      ?= .

DEFINES     :=
LDFLAGS     :=
INCLUDES    :=
LIBS	    :=
OBJS	    :=
PROF        :=

MODULES     :=
MODULE_DIRS :=

DISTNAME    := stella-snapshot

# Load the make rules generated by configure
include config.mak

# Uncomment this for stricter compile time code verification
# CXXFLAGS+= -Werror

ifdef CXXFLAGS
  CXXFLAGS:= $(CXXFLAGS)
else
  CXXFLAGS:= -O2
endif
CXXFLAGS+= -Wall 
ifdef HAVE_GCC
  CXXFLAGS+= -Wno-multichar -Wunused -fno-rtti -Woverloaded-virtual -Wnon-virtual-dtor -std=c++11
endif

ifdef PROFILE
  PROF:= -g -pg -fprofile-arcs -ftest-coverage
  CXXFLAGS+= $(PROF)
else
  ifdef HAVE_GCC
    CXXFLAGS+= -fomit-frame-pointer
  endif
endif

# Even more warnings...
#CXXFLAGS+= -pedantic -Wpointer-arith -Wcast-qual -Wconversion
#CXXFLAGS+= -Wshadow -Wimplicit -Wundef -Wnon-virtual-dtor
#CXXFLAGS+= -Wno-reorder -Wwrite-strings -fcheck-new -Wctor-dtor-privacy 

#######################################################################
# Misc stuff - you should never have to edit this                     #
#######################################################################

EXECUTABLE  := stella$(EXEEXT)

all: $(EXECUTABLE)


######################################################################
# Various minor settings
######################################################################

# The name for the directory used for dependency tracking
DEPDIR := .deps


######################################################################
# Module settings
######################################################################

MODULES := $(MODULES)

# After the game specific modules follow the shared modules
MODULES += \
	src/emucore \
	src/gui \
	src/common \
	src/common/tv_filters

######################################################################
# The build rules follow - normally you should have no need to
# touch whatever comes after here.
######################################################################

# Concat DEFINES and INCLUDES to form the CPPFLAGS
CPPFLAGS:= $(DEFINES) $(INCLUDES)

# Include the build instructions for all modules
-include $(addprefix $(srcdir)/, $(addsuffix /module.mk,$(MODULES)))

# Depdir information
DEPDIRS = $(addsuffix /$(DEPDIR),$(MODULE_DIRS))
DEPFILES = 

# The build rule for the Stella executable
$(EXECUTABLE):  $(OBJS)
	$(LD) $(LDFLAGS) $(PRE_OBJS_FLAGS) $+ $(POST_OBJS_FLAGS) $(LIBS) $(PROF) -o $@

distclean: clean
	$(RM_REC) $(DEPDIRS)
	$(RM) build.rules config.h config.mak config.log

clean:
	$(RM) $(OBJS) $(EXECUTABLE)

.PHONY: all clean dist distclean

.SUFFIXES: .cxx


ifndef CXX_UPDATE_DEP_FLAG
# If you use GCC, disable the above and enable this for intelligent
# dependency tracking. 
CXX_UPDATE_DEP_FLAG = -Wp,-MMD,"$(*D)/$(DEPDIR)/$(*F).d2"
.cxx.o:
	$(MKDIR) $(*D)/$(DEPDIR)
	$(CXX) $(CXX_UPDATE_DEP_FLAG) $(CXXFLAGS) $(CPPFLAGS) -c $(<) -o $*.o
	$(ECHO) "$(*D)/" > $(*D)/$(DEPDIR)/$(*F).d
	$(CAT) "$(*D)/$(DEPDIR)/$(*F).d2" >> "$(*D)/$(DEPDIR)/$(*F).d"
	$(RM) "$(*D)/$(DEPDIR)/$(*F).d2"

.c.o:
	$(MKDIR) $(*D)/$(DEPDIR)
	$(CXX) $(CXX_UPDATE_DEP_FLAG) $(CXXFLAGS) $(CPPFLAGS) -c $(<) -o $*.o
	$(ECHO) "$(*D)/" > $(*D)/$(DEPDIR)/$(*F).d
	$(CAT) "$(*D)/$(DEPDIR)/$(*F).d2" >> "$(*D)/$(DEPDIR)/$(*F).d"
	$(RM) "$(*D)/$(DEPDIR)/$(*F).d2"
else
# If you even have GCC 3.x, you can use this build rule, which is safer; the above
# rule can get you into a bad state if you Ctrl-C at the wrong moment.
# Also, with this GCC inserts additional dummy rules for the involved headers,
# which ensures a smooth compilation even if said headers become obsolete.
.cxx.o:
	$(MKDIR) $(*D)/$(DEPDIR)
	$(CXX) $(CXX_UPDATE_DEP_FLAG) $(CXXFLAGS) $(CPPFLAGS) -c $(<) -o $*.o

.c.o:
	$(MKDIR) $(*D)/$(DEPDIR)
	$(CXX) $(CXX_UPDATE_DEP_FLAG) $(CXXFLAGS) $(CPPFLAGS) -c $(<) -o $*.o
endif

# Include the dependency tracking files. We add /dev/null at the end
# of the list to avoid a warning/error if no .d file exist
-include $(wildcard $(addsuffix /*.d,$(DEPDIRS))) /dev/null

# check if configure has been run or has been changed since last run
config.mak: $(srcdir)/configure
	@echo "You need to run ./configure before you can run make"
	@echo "Either you haven't run it before or it has changed."
	@exit 1

install: all
	$(INSTALL) -d "$(DESTDIR)$(BINDIR)"
	$(INSTALL) -c -s -m 755 "$(srcdir)/stella$(EXEEXT)" "$(DESTDIR)$(BINDIR)/stella$(EXEEXT)"
	$(INSTALL) -d "$(DESTDIR)$(DOCDIR)"
	$(INSTALL) -c -m 644 "$(srcdir)/Announce.txt" "$(srcdir)/Changes.txt" "$(srcdir)/Copyright.txt" "$(srcdir)/License.txt" "$(srcdir)/README-SDL.txt" "$(srcdir)/Readme.txt" "$(srcdir)/Todo.txt" "$(srcdir)/docs/index.html" "$(srcdir)/docs/debugger.html" "$(DESTDIR)$(DOCDIR)/"
	$(INSTALL) -d "$(DESTDIR)$(DOCDIR)/graphics"
	$(INSTALL) -c -m 644 $(wildcard $(srcdir)/docs/graphics/*.png) "$(DESTDIR)$(DOCDIR)/graphics"
	$(INSTALL) -d "$(DESTDIR)$(DATADIR)/applications"
	$(INSTALL) -c -m 644 "$(srcdir)/src/unix/stella.desktop" "$(DESTDIR)$(DATADIR)/applications"
	$(INSTALL) -d "$(DESTDIR)$(DATADIR)/icons/hicolor/16x16/apps"
	$(INSTALL) -d "$(DESTDIR)$(DATADIR)/icons/hicolor/22x22/apps"
	$(INSTALL) -d "$(DESTDIR)$(DATADIR)/icons/hicolor/24x24/apps"
	$(INSTALL) -d "$(DESTDIR)$(DATADIR)/icons/hicolor/32x32/apps"
	$(INSTALL) -d "$(DESTDIR)$(DATADIR)/icons/hicolor/48x48/apps"
	$(INSTALL) -d "$(DESTDIR)$(DATADIR)/icons/hicolor/64x64/apps"
	$(INSTALL) -d "$(DESTDIR)$(DATADIR)/icons/hicolor/128x128/apps"
	$(INSTALL) -c -m 644 "$(srcdir)/src/common/stella-16x16.png" "$(DESTDIR)$(DATADIR)/icons/hicolor/16x16/apps/stella.png"
	$(INSTALL) -c -m 644 "$(srcdir)/src/common/stella-22x22.png" "$(DESTDIR)$(DATADIR)/icons/hicolor/22x22/apps/stella.png"
	$(INSTALL) -c -m 644 "$(srcdir)/src/common/stella-24x24.png" "$(DESTDIR)$(DATADIR)/icons/hicolor/24x24/apps/stella.png"
	$(INSTALL) -c -m 644 "$(srcdir)/src/common/stella-32x32.png" "$(DESTDIR)$(DATADIR)/icons/hicolor/32x32/apps/stella.png"
	$(INSTALL) -c -m 644 "$(srcdir)/src/common/stella-48x48.png" "$(DESTDIR)$(DATADIR)/icons/hicolor/48x48/apps/stella.png"
	$(INSTALL) -c -m 644 "$(srcdir)/src/common/stella-64x64.png" "$(DESTDIR)$(DATADIR)/icons/hicolor/64x64/apps/stella.png"
	$(INSTALL) -c -m 644 "$(srcdir)/src/common/stella-128x128.png" "$(DESTDIR)$(DATADIR)/icons/hicolor/128x128/apps/stella.png"

install-strip: install
	$(STRIP) stella$(EXEEXT)

uninstall:
	rm -f  "$(DESTDIR)$(BINDIR)/stella$(EXEEXT)"
	rm -rf "$(DESTDIR)$(DOCDIR)/"
	rm -f  "$(DESTDIR)$(DATADIR)/applications/stella.desktop"
	rm -f  "$(DESTDIR)$(DATADIR)/icons/hicolor/16x16/apps/stella.png"
	rm -f  "$(DESTDIR)$(DATADIR)/icons/hicolor/22x22/apps/stella.png"
	rm -f  "$(DESTDIR)$(DATADIR)/icons/hicolor/24x24/apps/stella.png"
	rm -f  "$(DESTDIR)$(DATADIR)/icons/hicolor/32x32/apps/stella.png"
	rm -f  "$(DESTDIR)$(DATADIR)/icons/hicolor/48x48/apps/stella.png"
	rm -f  "$(DESTDIR)$(DATADIR)/icons/hicolor/64x64/apps/stella.png"
	rm -f  "$(DESTDIR)$(DATADIR)/icons/hicolor/128x128/apps/stella.png"

# Special rule for M6502.ins, generated from m4 (there's probably a better way to do this ...)
src/emucore/M6502.ins: src/emucore/M6502.m4
	m4 src/emucore/M6502.m4 > src/emucore/M6502.ins

# Special rule for windows icon stuff (there's probably a better way to do this ...)
src/windows/stella_icon.o: src/windows/stella.ico src/windows/stella.rc
	windres --include-dir src/windows src/windows/stella.rc src/windows/stella_icon.o 
		
.PHONY: deb bundle test install uninstall
