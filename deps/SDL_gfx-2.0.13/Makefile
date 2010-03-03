INCLUDE = -I../SDL-1.2.13/include
CFLAGS  = -g -O2 $(INCLUDE)
AR	= ar
RANLIB	= ranlib

TARGET = libSDL_gfx.a

HEADERS =	\
	SDL_gfxPrimitives.h	\
	SDL_gfxPrimitives_font.h	\
	SDL_rotozoom.h	\
	SDL_framerate.h \
	SDL_imageFilter.h

SOURCES =		\
	SDL_gfxPrimitives.c	\
	SDL_rotozoom.c	\
	SDL_framerate.c \
	SDL_imageFilter.c

OBJECTS = $(shell echo $(SOURCES) | sed -e 's,\.c,\.o,g')

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) crv $@ $^
	$(RANLIB) $@

clean:
	#rm -f $(TARGET) $(OBJECTS)
	rm -f $(OBJECTS)
