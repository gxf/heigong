
SUBDIRS=lib prog build

all:
	@set -e; \
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir; done

clean:
	@set -e; \
	for dir in $(SUBDIRS); do $(MAKE) clean -C $$dir; done

hgtest: test.o libhg.a
	g++ $< -L. -lhg -lGL -lGLU -lGLEW -lSDL -lfreetype -lpng -ljpeg -o $@

test.o: test.cpp inc/hg.h
	g++ -O0 -g -c $< -o $@
