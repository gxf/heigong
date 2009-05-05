CC=g++
CFLAGS=-g
#INC=
INC_PATH=-I/usr/include/libgsf-1/ -I/usr/include/glib-2.0/
LIBS=-lwv -lgsf -lglib
LIB_PATH=

VPATH=

all: HG_main.cpp
	$(CC) $(CFLAGS) $(INC_PATH) $(LIB_PATH) $(LIBS) $< -o $@
