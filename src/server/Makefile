DESTDIR=
PREFIX=/usr
INSTALLDIR=$(DESTDIR)$(PREFIX)

CFLAGS = -O2 -Wall -Wextra -I../
ifdef MAC
CFLAGS += -D_MAC
else
CFLAGS += -pthread
endif

CC=gcc
CXX=g++
CXXFLAGS = $(CFLAGS)

OBJS  = ../../build/obj/server/usercon.o
OBJS += ../../build/obj/server/ninjamsrv.o


../../build/obj/server/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

default: ninjamsrv

ninjamsrv: $(OBJS)
	$(CXX) $(CXXFLAGS) -o ../../build/ninjamsrv $(OBJS) -L../../build/lib -lninjam-common

clean:
	-rm $(OBJS) ../../build/ninjamsrv

install: default
	mkdir -p                               $(INSTALLDIR)/bin
	mkdir -p                               $(INSTALLDIR)/usr/lib/libninjam
	install -m755 ../../build/ninjamsrv    $(INSTALLDIR)/bin
	install -m644 cc-by-sa-4-license.txt   $(INSTALLDIR)/lib/libninjam
	install -m644 jams_license.txt.example $(INSTALLDIR)/lib/libninjam
	install -m644 ninjamsrv.cfg.example    $(INSTALLDIR)/lib/libninjam
