CC     = gcc
CFLAGS = -O2 -Wall
INCDIR = -I/usr/X11R6/include
DESTDIR= /usr/X11R6
LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lX11 -lXext
OBJS   = wmpowercon.o \
         ../wmgeneral/wmgeneral.o

.c.o:
	$(CC) $(COPTS) -D$(shell echo `uname -s`) -c $< -o $*.o $(INCDIR)

all:	wmpowercon.o wmpowercon

wmpowercon.o: wmapm_master.xpm wmapm_mask.xbm wmpowercon.h
wmpowercon: $(OBJS)
	$(CC) $(COPTS) $(SYSTEM) -o wmpowercon $^ $(INCDIR) $(LIBDIR) $(LIBS)

clean:
	for i in $(OBJS) ; do \
		rm -f $$i; \
	done
	rm -f wmpowercon

install:: wmpowercon
	install -s -m 0755 wmpowercon $(DESTDIR)/bin
	install    -m 0644 wmpowercon.1 $(DESTDIR)/man/man1 
	chmod +s $(DESTDIR)/bin/wmpowercon
