CC=gcc
CFLAGS=-g -I/usr/include/libxml2
LDFLAGS=-g -lxml2

OBJECTS=chm_lib.o lzx.o
HEADERS=chm_lib.h lzx.h

TARGET=listchm takechm
all:$(TARGET)

listchm:listchm.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

takechm:takechm.o $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o:%.c $(HEADERS)

clean:
	rm -f *.o $(TARGET)
