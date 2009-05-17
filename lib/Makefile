HG_ROOT=..
BUILD_DIR=$(HG_ROOT)/build
INC_DIR=$(HG_ROOT)/inc

include $(HG_ROOT)/makefile.def

TARGETS=libhg.a

OBJECTS=MayTwelfth.o 	\
		RenderMan.o 	\
		FontsManager.o 	\
		LayoutManager.o \
		FontsCache.o 	\
		DocParser.o		\
		Char.o

VPATH=$(INC_DIR):$(BUILD_DIR)

all: $(TARGETS) 

libhg.a: $(OBJECTS)
	$(AR) $(BUILD_DIR)/$@  $(addprefix $(BUILD_DIR)/,$^)

RenderMan.o: RenderMan.cpp Logger.h Color.h RenderMan.h Common.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(BUILD_DIR)/$@

FontsManager.o: FontsManager.cpp Logger.h FontsManager.h Common.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(BUILD_DIR)/$@

FontsCache.o: FontsCache.cpp FontsCache.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(BUILD_DIR)/$@

LayoutManager.o: LayoutManager.cpp Logger.h FontsManager.h LayoutManager.h Common.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(BUILD_DIR)/$@

MayTwelfth.o: MayTwelfth.cpp MayTwelfth.h Logger.h RenderMan.h FontsManager.h LayoutManager.h DocParser.h Common.h Glyph.h Line.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(BUILD_DIR)/$@

DocParser.o: DocParser.cpp DocParser.h Logger.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(BUILD_DIR)/$@

Char.o: Char.cpp Glyph.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(BUILD_DIR)/$@

clean:
	$(RM) $(addprefix $(BUILD_DIR)/,$(OBJECTS))
	$(RM) $(BUILD_DIR)/$(TARGETS)
