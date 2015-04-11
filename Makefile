DESTDIR=
PREFIX=/usr
INSTALLDIR=$(DESTDIR)$(PREFIX)
VERSION=$(shell head -n 1 debian/changelog|sed -e "s,^ninjam (\(.*\)) .*,\1,")

#############################################################
# CPU optimization section
#############################################################

OPTFLAGS = -O2
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
#CFLAGS = $(OPTFLAGS) -s -Wall -Wextra -Isrc/








CFLAGS = $(OPTFLAGS) -s -Isrc/
# CFLAGS += -Wshadow
CC=gcc
CXX=g++
CXXFLAGS = $(CFLAGS)

COMMON_OBJS  = build/obj/WDL/rng.o
COMMON_OBJS += build/obj/WDL/sha.o
COMMON_OBJS += build/obj/WDL/jnetlib/asyncdns.o
COMMON_OBJS += build/obj/WDL/jnetlib/connection.o
COMMON_OBJS += build/obj/WDL/jnetlib/listen.o
COMMON_OBJS += build/obj/WDL/jnetlib/util.o
COMMON_OBJS += build/obj/WDL/jnetlib/httpget.o
COMMON_OBJS += build/obj/ninjam/mpb.o
COMMON_OBJS += build/obj/ninjam/netmsg.o

CLIENT_OBJS  = build/obj/ninjam/njclient.o
CLIENT_OBJS += build/obj/ninjam/njmisc.o
ifdef MAC
CLIENT_OBJS += build/obj/ninjam/audiostream_mac.o
else
CLIENT_OBJS += build/obj/ninjam/audiostream_alsa.o
CLIENT_OBJS += build/obj/ninjam/audiostream_jack.o
LFLAGS += -ljack -lasound
endif


build/obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

default: libninjam-common.so libninjam-client.so \
	       libninjam-common.a  libninjam-client.a  \
	       ninjam.pc           ninjam-client.pc
	$(MAKE) -C src/cursesclient
	$(MAKE) -C src/server

libninjam-common.so: $(COMMON_OBJS)
	$(CXX) -shared $(CXXFLAGS) -o build/lib/libninjam-common.so $(COMMON_OBJS)

libninjam-client.so: $(CLIENT_OBJS) build/lib/libninjam-common.so
	$(CXX) -shared $(CXXFLAGS) -o build/lib/libninjam-client.so $(CLIENT_OBJS) \
	$(LFLAGS) -logg -lvorbis -lvorbisenc -Lbuild/lib/ -lninjam-common

libninjam-common.a: $(COMMON_OBJS)
	ar cr build/lib/libninjam-common.a $(COMMON_OBJS)

libninjam-client.a: $(CLIENT_OBJS) libninjam-common.a
	ar cr build/lib/libninjam-client.a $(CLIENT_OBJS) build/lib/libninjam-common.a

%.pc: build/pc/%.pc.in
	sed 's,^\(prefix=\).*,\1$(PREFIX),; s,^\(Version: \).*,\1$(VERSION),' $< > build/pc/$@

clean:
	-rm $(COMMON_OBJS)                $(CLIENT_OBJS)                \
	    build/lib/libninjam-common.so build/lib/libninjam-client.so \
	    build/lib/libninjam-common.a  build/lib/libninjam-client.a  \
	    build/pc/ninjam.pc            build/pc/ninjam-client.pc
	$(MAKE) -C src/cursesclient clean
	$(MAKE) -C src/server       clean

install: default
	mkdir   -p                               $(INSTALLDIR)/include/libninjam/ninjam
	mkdir   -p                               $(INSTALLDIR)/include/libninjam/WDL/jnetlib
	mkdir   -p                               $(INSTALLDIR)/lib/pkgconfig
	install -m 644 src/ninjam/*.h            $(INSTALLDIR)/include/libninjam/ninjam/
	install -m 644 src/WDL/*.h               $(INSTALLDIR)/include/libninjam/WDL/
	install -m 644 src/WDL/jnetlib/*.h       $(INSTALLDIR)/include/libninjam/WDL/jnetlib/
	install        build/lib/*.so            $(INSTALLDIR)/lib/
	install -m 755 build/lib/*.a             $(INSTALLDIR)/lib/
	install -m 644 build/pc/ninjam.pc        $(INSTALLDIR)/lib/pkgconfig/
	install -m 644 build/pc/ninjam-client.pc $(INSTALLDIR)/lib/pkgconfig/
	$(MAKE) -C src/cursesclient install
	$(MAKE) -C src/server       install

