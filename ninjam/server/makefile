DESTDIR=/
PREFIX=/usr
INSTALLDIR=$(DESTDIR)$(PREFIX)

CFLAGS =  -O2 -s -Wall -I../..

ifdef MAC
CFLAGS += -D_MAC
else
CFLAGS += -pthread
endif

CC=gcc
CXX=g++
CXXFLAGS = $(CFLAGS)

OBJS = usercon.o
OBJS += ninjamsrv.o

default: ninjamsrv

ninjamsrv: $(OBJS)
	$(CXX) $(CXXFLAGS) -o ninjamsrv $(OBJS) -L../.. -lninjam-common

clean:
	-rm $(OBJS) ninjamsrv

install: ninjamsrv
	mkdir -p $(INSTALLDIR)/bin
	install ninjamsrv $(INSTALLDIR)/bin
