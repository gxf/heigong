HG_ROOT=..
BUILD_DIR=$(HG_ROOT)/build
INC_DIR=$(HG_ROOT)/inc

include $(HG_ROOT)/makefile.def

VPATH=$(INC_DIR):$(BUILD_DIR)

TARGETS=libprog.a

CPP_SRC_FILES=$(shell ls *.cpp)
SRC_FILES=$(CPP_SRC_FILES)
OBJ_FILES=$(subst .cpp,.o,$(CPP_SRC_FILES))
DEP_FILES=$(addprefix $(BUILD_DIR)/,$(subst .cpp,.d,$(CPP_SRC_FILES)))	\
		  $(addprefix $(BUILD_DIR)/,$(subst .c,.d,$(C_SRC_FILES)))

all: $(TARGETS)

libprog.a: $(OBJ_FILES)
	$(AR) $(BUILD_DIR)/$@  $(addprefix $(BUILD_DIR)/,$^)

-include $(DEP_FILES)

$(BUILD_DIR)/%.d: %.cpp
	@set -e; rm -f $(BUILD_DIR)/$@; \
	$(CC) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $(BUILD_DIR)/$@ : ,g' > $(BUILD_DIR)/$@; \
	echo "	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(subst .d,.o,$@) " >> $(BUILD_DIR)/$@

clean:
	$(RM) $(addprefix $(BUILD_DIR)/,$(OBJ_FILES))
	$(RM) $(DEP_FILES)
	$(RM) $(BUILD_DIR)/$(TARGETS)
