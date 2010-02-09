CFLAGS=-g -Wall -I.

TARGET=libzip.a
OBJECTS=$(subst .c,.o,$(shell echo *.c))
INCLUDES=$(shell echo *.h *.h)

all:$(TARGET)

$(TARGET):$(OBJECTS)
	$(AR) cru $@ $^
	ranlib $@

%.o:%.c $(INCLUDES) Makefile
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJECTS)

