HG_ROOT=..
BUILD_DIR=$(HG_ROOT)/build
INC_DIR=$(HG_ROOT)/inc

include $(HG_ROOT)/makefile.def

VPATH=$(INC_DIR):$(BUILD_DIR)

TARGETS=libprog.a

OBJECTS=hgMaster.o

all: $(TARGETS)

libprog.a: hgMaster.o
	$(AR) $(BUILD_DIR)/$@  $(addprefix $(BUILD_DIR)/,$^)

hgMaster.o: hgMaster.cpp MayTwelfth.h Logger.h RenderMan.h FontsManager.h LayoutManager.h DocParser.h Common.h Glyph.h Line.h
	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(BUILD_DIR)/$@

clean:
	$(RM) $(addprefix $(BUILD_DIR)/,$(OBJECTS))
	$(RM) $(BUILD_DIR)/$(TARGETS)
