ifndef PREFIX
  PREFIX=/usr
endif
INSTALLDIR=$(DESTDIR)$(PREFIX)
VERSION=$(shell head -n 1 debian/changelog | sed 's|^ninjam (\(.*\)-.*).*|\1|')


CC      = gcc
CXX     = g++
CFLAGS  = -Wall -Wextra
LFLAGS  = -lpthread -lm -logg -lvorbis -lvorbisenc
LFLAGS += -L. -lninjam-common
ifdef MAC
  CFLAGS += -D_MAC
  LFLAGS += -framework coreaudio
else
  CFLAGS += -pthread
  ifeq ($(shell uname -m), x86_64)
    CFLAGS += -fPIC
  else
    CFLAGS += -malign-double
  endif
  LFLAGS += -ljack -lasound
endif
CXXFLAGS = $(CFLAGS) $(CPPFLAGS)

COMMON_OBJS  = WDL/jnetlib/asyncdns.o
COMMON_OBJS += WDL/jnetlib/connection.o
COMMON_OBJS += WDL/jnetlib/httpget.o
COMMON_OBJS += WDL/jnetlib/listen.o
COMMON_OBJS += WDL/jnetlib/util.o
COMMON_OBJS += WDL/rng.o
COMMON_OBJS += WDL/sha.o
COMMON_OBJS += ninjam/mpb.o
COMMON_OBJS += ninjam/netmsg.o

CLIENT_OBJS  = ninjam/njclient.o
CLIENT_OBJS += ninjam/njmisc.o
ifdef MAC
  CLIENT_OBJS += ninjam/audiostream_mac.o
else
  CLIENT_OBJS += ninjam/audiostream_alsa.o
  CLIENT_OBJS += ninjam/audiostream_jack.o
endif

ARTIFACTS = libninjam-common.so libninjam-common.a ninjam.pc        \
            libninjam-client.so libninjam-client.a ninjam-client.pc


default: $(ARTIFACTS)
	$(MAKE) -C ninjam/cursesclient
	$(MAKE) -C ninjam/server

libninjam-common.so: $(COMMON_OBJS)
	$(CXX) -shared $(CXXFLAGS) -o libninjam-common.so $(COMMON_OBJS)

libninjam-client.so: $(CLIENT_OBJS) libninjam-common.so
	$(CXX) -shared $(CXXFLAGS) -o libninjam-client.so $(CLIENT_OBJS) $(LFLAGS)

libninjam-common.a: $(COMMON_OBJS)
	ar cr libninjam-common.a $(COMMON_OBJS)

libninjam-client.a: $(CLIENT_OBJS) libninjam-common.a
	ar cr libninjam-client.a $(CLIENT_OBJS) libninjam-common.a

%.pc: %.pc.in
	sed 's,^\(prefix=\).*,\1$(PREFIX),; s,^\(Version: \).*,\1$(VERSION),' $< >$@

clean:
	-rm $(COMMON_OBJS) $(CLIENT_OBJS) $(ARTIFACTS)
	$(MAKE) -C ninjam/cursesclient clean
	$(MAKE) -C ninjam/server       clean

install: $(ARTIFACTS)
	mkdir -p $(INSTALLDIR)/include/libninjam/ninjam
	mkdir -p $(INSTALLDIR)/include/libninjam/WDL/jnetlib
	mkdir -p $(INSTALLDIR)/lib/pkgconfig
	install -m644 ninjam/*.h       $(INSTALLDIR)/include/libninjam/ninjam/
	install -m644 WDL/*.h          $(INSTALLDIR)/include/libninjam/WDL/
	install -m644 WDL/jnetlib/*.h  $(INSTALLDIR)/include/libninjam/WDL/jnetlib/
	install -m644 *.so             $(INSTALLDIR)/lib/
	install -m644 *.a              $(INSTALLDIR)/lib/
	install -m644 *.pc             $(INSTALLDIR)/lib/pkgconfig/
	$(MAKE) -C ninjam/cursesclient install
	$(MAKE) -C ninjam/server       install

uninstall:
	rm -rf $(INSTALLDIR)/include/libninjam/
	rm     $(INSTALLDIR)/lib/libninjam-*.a
	rm     $(INSTALLDIR)/lib/libninjam-*.so
	rm     $(INSTALLDIR)/lib/pkgconfig/ninjam-*.pc
	$(MAKE) -C ninjam/cursesclient uninstall
	$(MAKE) -C ninjam/server       uninstall
