PREFIX=$(DESTDIR)/usr

#############################################################
# CPU optimization section
#############################################################

OPTFLAGS =  -O2

ifdef MAC
OPTFLAGS += -D_MAC -mcpu=7450
LFLAGS = -framework coreaudio -lncurses.5 -lm
else
OPTFLAGS += -pthread -malign-double 
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
	 libninjam-common.a libninjam-client.a
	(cd ninjam/cursesclient && make)
	(cd ninjam/server && make)

libninjam-common.so: $(COMMON_OBJS)
	$(CXX) -shared $(CXXFLAGS) -o libninjam-common.so $(COMMON_OBJS)

libninjam-client.so: $(CLIENT_OBJS) libninjam-common.so
	$(CXX) -shared $(CXXFLAGS) -o libninjam-client.so $(CLIENT_OBJS) \
	$(LFLAGS) -logg -lvorbis -lvorbisenc -L. -lninjam-common

libninjam-common.a: $(COMMON_OBJS)
	ar cr libninjam-common.a $(COMMON_OBJS)

libninjam-client.a: $(CLIENT_OBJS) libninjam-common.a
	ar cr libninjam-client.a $(CLIENT_OBJS) libninjam-common.a

clean:
	-rm $(COMMON_OBJS) $(CLIENT_OBJS) \
	libninjam-common.so libninjam-client.so \
	libninjam-common.a libninjam-client.a
	(cd ninjam/cursesclient && make clean)
	(cd ninjam/server && make clean)

install: libninjam-common.so libninjam-client.so libninjam-common.a libninjam-client.a
	mkdir -p $(PREFIX)/include/libninjam/ninjam
	mkdir -p $(PREFIX)/include/libninjam/WDL
	mkdir -p $(PREFIX)/include/libninjam/WDL/jnetlib
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/lib/pkgconfig
	install -m 644 ninjam/*.h $(PREFIX)/include/libninjam/ninjam/
	install -m 644 WDL/*.h $(PREFIX)/include/libninjam/WDL/
	install -m 644 WDL/jnetlib/*.h $(PREFIX)/include/libninjam/WDL/jnetlib/
	install *.so $(PREFIX)/lib/
	install -m 755 *.a $(PREFIX)/lib/
	install -m 644 ninjam.pc $(PREFIX)/lib/pkgconfig/
	(cd ninjam/cursesclient && make install)
	(cd ninjam/server && make install)