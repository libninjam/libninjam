ifndef PREFIX
  PREFIX=/usr
endif
INSTALLDIR=$(DESTDIR)$(PREFIX)

CC     = gcc
CXX    = g++
CFLAGS = -Wall -Wextra
LFLAGS = -L../../ -lninjam-common
ifdef MAC
  CFLAGS += -D_MAC
else
  ifneq ($(shell uname -m), x86_64)
    CFLAGS += -malign-double
  endif
endif
CXXFLAGS = $(CFLAGS) $(CPPFLAGS)

OBJS  = usercon.o
OBJS += ninjamsrv.o


default: ninjamsrv

ninjamsrv: $(OBJS)
	$(CXX) $(CXXFLAGS) -o ninjamsrv $(OBJS) $(LFLAGS)

clean:
	-rm $(OBJS) ninjamsrv

install: default
	mkdir -p                               $(INSTALLDIR)/bin
	mkdir -p                               $(INSTALLDIR)/usr/lib/libninjam
	install -m755 ninjamsrv                $(INSTALLDIR)/bin
	install -m644 cclicense.txt            $(INSTALLDIR)/lib/libninjam
# 	install -m644 cc-by-sa-4-license.txt   $(INSTALLDIR)/lib/libninjam
# 	install -m644 jams_license.txt.example $(INSTALLDIR)/lib/libninjam
# 	install -m644 ninjamsrv.cfg.example    $(INSTALLDIR)/lib/libninjam

uninstall:
	rm $(INSTALLDIR)/bin/ninjamsrv
	rm $(INSTALLDIR)/lib/libninjam/cclicense.txt
# 	rm $(INSTALLDIR)/lib/libninjam/cc-by-sa-4-license.txt
# 	rm $(INSTALLDIR)/lib/libninjam/jams_license.txt.example
# 	rm $(INSTALLDIR)/lib/libninjam/ninjamsrv.cfg.example
