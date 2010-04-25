CFLAGS=-g -I../libzip `xml2-config --cflags`
LDFLAGS=$(CFLAGS) -lz -L../libzip -lzip `xml2-config --libs`

TARGET=$(shell basename `pwd`)
OBJECTS=$(subst .c,.o,$(shell echo *.c))
INCLUDES=$(shell echo *.h)

all:$(TARGET)

$(TARGET):$(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

%.o:%.c $(INCLUDES) Makefile
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJECTS)
