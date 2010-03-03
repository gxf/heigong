# Makefile to build the SDL tests

srcdir  = .

CFLAGS  = -I.. -I../../SDL-1.2.13/include  -g
LIBS	= -L. -lSDL -lSDL_gfx -pthread -lm

TARGETS = TestFramerate TestGfxPrimitives

all: $(TARGETS)

$(TARGETS):libSDL.a libSDL_gfx.a

libSDL.a:../../SDL-1.2.13/libSDL.a
	cp -f ../../SDL-1.2.13/libSDL.a .

../../SDL-1.2.13/libSDL.a:
	(cd ../../SDL-1.2.13/; make)

libSDL_gfx.a:../libSDL_gfx.a
	cp -f ../libSDL_gfx.a .

../libSDL_gfx.a:
	(cd ..; make)

TestFramerate: $(srcdir)/TestFramerate.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

TestGfxPrimitives: $(srcdir)/TestGfxPrimitives.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGETS) libSDL.a libSDL_gfx.a

distclean: clean
	rm -f Makefile
