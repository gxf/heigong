
CC=g++
CFLAGS=-g -Wall
LD=g++
LDFLAGS=
#INC=
INC_PATH=-I/usr/include/GL -I/usr/include/freetype2
LIBS=-lGL -lGLU -lGLEW -lSDL -lfreetype
LIB_PATH=

RM=rm -f

TARGETS=hgMaster

OBJECTS=hgMaster.o 	\
		MayTwelfth.o 	\
		RenderMan.o 	\
		FontsManager.o 	\
		LayoutManager.o \
		FontsCache.o 	\
		DocParser.o		\
		Char.o

VPATH=

all: $(TARGETS) 

hgMaster: $(OBJECTS)
	$(LD) $(LDFLAGS) $(LIB_PATH) $(LIBS) $^ -o $@

hgMaster.o: hgMaster.cpp Logger.h Color.h RenderMan.h MayTwelfth.h Common.h FontsCache.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

RenderMan.o: RenderMan.cpp Logger.h Color.h RenderMan.h Common.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

FontsManager.o: FontsManager.cpp Logger.h FontsManager.h Common.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

FontsCache.o: FontsCache.cpp FontsCache.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

LayoutManager.o: LayoutManager.cpp Logger.h FontsManager.h LayoutManager.h Common.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

MayTwelfth.o: MayTwelfth.cpp MayTwelfth.h Logger.h RenderMan.h FontsManager.h LayoutManager.h Common.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

DocParser.o: DocParser.cpp DocParser.h Logger.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

Char.o: Char.cpp Glyph.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $@

clean:
	$(RM) *.o
	$(RM) hgMaster
