# Makefile to build the SDL tests

srcdir  = .

CFLAGS  = -I../include -g
LIBS	= -L. -lSDL -pthread

TARGETS = testwin testsprite

all: $(TARGETS)

$(TARGETS):libSDL.a

libSDL.a:../libSDL.a
	cp -f ../libSDL.a .

../libSDL.a:
	(cd ..; make)

testsprite: $(srcdir)/testsprite.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -lm

testwin: $(srcdir)/testwin.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGETS) libSDL.a

distclean: clean
	rm -f Makefile
