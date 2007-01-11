DESTDIR=/
PREFIX=/usr
INSTALLDIR=$(DESTDIR)$(PREFIX)

#############################################################
# CPU optimization section
#############################################################

OPTFLAGS =  -O2

LFLAGS = -lm -L../.. -lninjam-client -lninjam-common
ifdef MAC
OPTFLAGS += -D_MAC -mcpu=7450
LFLAGS += -framework coreaudio -lncurses.5
else
OPTFLAGS += -malign-double 
LFLAGS += -lncurses
endif

#############################################################
# Basic Configuration
#############################################################

# we MUST have -fomit-frame-pointer and -lm, otherwise we hate life
CFLAGS = $(OPTFLAGS) -s -I../..
# CFLAGS += -Wshadow
CC=gcc
CXX=g++

OBJS = cursesclient.o


CXXFLAGS = $(CFLAGS)

default: cninjam

cninjam: $(OBJS)
	$(CXX) $(CXXFLAGS) -o cninjam $(OBJS) -lpthread $(LFLAGS)

clean:
	-rm $(OBJS) cninjam

install: cninjam
	mkdir -p $(INSTALLDIR)/bin
	install cninjam $(INSTALLDIR)/bin
