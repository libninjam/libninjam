DESTDIR=
PREFIX=/usr
INSTALLDIR=$(DESTDIR)$(PREFIX)
VERSION=$(shell head -n 1 debian/changelog|sed -e "s,^ninjam (\(.*\)) .*,\1,")

#############################################################
# CPU optimization section
#############################################################

OPTFLAGS =  -O2

ifdef MAC
OPTFLAGS += -D_MAC -mcpu=7450
LFLAGS = -framework coreaudio -lncurses.5 -lm
else
OPTFLAGS += -pthread
MACH=$(shell uname -m)
ifeq ($(MACH), x86_64)
OPTFLAGS += -fPIC
else
OPTFLAGS += -malign-double
endif
LFLAGS = -lm -lpthread
endif

#############################################################
# Basic Configuration
#############################################################

# we MUST have -fomit-frame-pointer and -lm, otherwise we hate life
CFLAGS = $(OPTFLAGS) -s -I.
# CFLAGS += -Wshadow
CC=gcc
CXX=g++

COMMON_OBJS = WDL/jnetlib/asyncdns.o
COMMON_OBJS += WDL/jnetlib/connection.o
COMMON_OBJS += WDL/jnetlib/listen.o
COMMON_OBJS += WDL/jnetlib/util.o
COMMON_OBJS += WDL/jnetlib/httpget.o
COMMON_OBJS += WDL/rng.o
COMMON_OBJS += WDL/sha.o
COMMON_OBJS += ninjam/mpb.o
COMMON_OBJS += ninjam/netmsg.o

CLIENT_OBJS = ninjam/njclient.o
ifdef MAC
CLIENT_OBJS += ninjam/audiostream_mac.o
else
CLIENT_OBJS += ninjam/audiostream_jack.o
CLIENT_OBJS += ninjam/audiostream_alsa.o
LFLAGS += -ljack -lasound
endif
CLIENT_OBJS += ninjam/njmisc.o

CXXFLAGS = $(CFLAGS)

default: libninjam-common.so libninjam-client.so \
	 libninjam-common.a libninjam-client.a \
	 ninjam.pc ninjam-client.pc
	$(MAKE) -C ninjam/cursesclient
	$(MAKE) -C ninjam/server

libninjam-common.so: $(COMMON_OBJS)
	$(CXX) -shared $(CXXFLAGS) -o libninjam-common.so $(COMMON_OBJS)

libninjam-client.so: $(CLIENT_OBJS) libninjam-common.so
	$(CXX) -shared $(CXXFLAGS) -o libninjam-client.so $(CLIENT_OBJS) \
	$(LFLAGS) -logg -lvorbis -lvorbisenc -L. -lninjam-common

libninjam-common.a: $(COMMON_OBJS)
	ar cr libninjam-common.a $(COMMON_OBJS)

libninjam-client.a: $(CLIENT_OBJS) libninjam-common.a
	ar cr libninjam-client.a $(CLIENT_OBJS) libninjam-common.a

%.pc: %.pc.in
	sed 's,^\(prefix=\).*,\1$(PREFIX),; s,^\(Version: \).*,\1$(VERSION),' $< >$@

clean:
	-rm $(COMMON_OBJS) $(CLIENT_OBJS) \
	libninjam-common.so libninjam-client.so \
	libninjam-common.a libninjam-client.a \
	ninjam.pc ninjam-client.pc
	$(MAKE) -C ninjam/cursesclient clean
	$(MAKE) -C ninjam/server clean

install: libninjam-common.so libninjam-client.so libninjam-common.a libninjam-client.a \
	 ninjam.pc ninjam-client.pc
	mkdir -p $(INSTALLDIR)/include/libninjam/ninjam
	mkdir -p $(INSTALLDIR)/include/libninjam/WDL
	mkdir -p $(INSTALLDIR)/include/libninjam/WDL/jnetlib
	mkdir -p $(INSTALLDIR)/lib
	mkdir -p $(INSTALLDIR)/lib/pkgconfig
	install -m 644 ninjam/*.h $(INSTALLDIR)/include/libninjam/ninjam/
	install -m 644 WDL/*.h $(INSTALLDIR)/include/libninjam/WDL/
	install -m 644 WDL/jnetlib/*.h $(INSTALLDIR)/include/libninjam/WDL/jnetlib/
	install *.so $(INSTALLDIR)/lib/
	install -m 755 *.a $(INSTALLDIR)/lib/
	install -m 644 ninjam.pc $(INSTALLDIR)/lib/pkgconfig/
	install -m 644 ninjam-client.pc $(INSTALLDIR)/lib/pkgconfig/
	$(MAKE) -C ninjam/cursesclient install
	$(MAKE) -C ninjam/server install
