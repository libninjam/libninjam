DESTDIR=
PREFIX=/usr
INSTALLDIR=$(DESTDIR)$(PREFIX)

#############################################################
# CPU optimization section
#############################################################

OPTFLAGS = -O2
LFLAGS = -lm -L../../build/lib -lninjam-client -lninjam-common
ifdef MAC
OPTFLAGS += -D_MAC -mcpu=7450
LFLAGS += -framework coreaudio -lncurses.5
else
MACH=$(shell uname -m)
ifeq ($(MACH), x86_64)
OPTFLAGS += -fPIC
else
OPTFLAGS += -malign-double
endif
LFLAGS += -lncurses
endif

#############################################################
# Basic Configuration
#############################################################

# we MUST have -fomit-frame-pointer and -lm, otherwise we hate life
CFLAGS = $(OPTFLAGS) -s -Wall -Wextra -I../
# CFLAGS += -Wshadow
CC=gcc
CXX=g++
CXXFLAGS = $(CFLAGS)

OBJS = ../../build/obj/cursesclient/cursesclient.o


../../build/obj/cursesclient/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

default: cninjam

cninjam: $(OBJS)
	$(CXX) $(CXXFLAGS) -o ../../build/cninjam $(OBJS) -lpthread $(LFLAGS)

clean:
	-rm $(OBJS) ../../build/cninjam

install: default
	mkdir -p                    $(INSTALLDIR)/bin
	install ../../build/cninjam $(INSTALLDIR)/bin

