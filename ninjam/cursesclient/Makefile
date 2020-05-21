ifndef PREFIX
  PREFIX=/usr
endif
INSTALLDIR=$(DESTDIR)$(PREFIX)

CC      = gcc
CXX     = g++
CFLAGS  = -Wall -Wextra
LFLAGS  = -lpthread -lm
LFLAGS += -L../../ -lninjam-client -lninjam-common
OBJS    = cursesclient.o
ifdef MAC
  CFLAGS += -D_MAC
  LFLAGS += -lncurses.5
else
  ifneq ($(shell uname -m), x86_64)
    CFLAGS += -malign-double
  endif
  LFLAGS += -lncurses
endif
CXXFLAGS = $(CFLAGS) $(CPPFLAGS)


default: cninjam

cninjam: $(OBJS)
	$(CXX) $(CXXFLAGS) -o cninjam $(OBJS) $(LFLAGS)

clean:
	-rm $(OBJS) cninjam

install: default
	mkdir -p        $(INSTALLDIR)/bin
	install cninjam $(INSTALLDIR)/bin

uninstall:
	rm $(INSTALLDIR)/bin/cninjam
