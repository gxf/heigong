HG_ROOT=..
BUILD_DIR=$(HG_ROOT)/build
INC_DIR=$(HG_ROOT)/inc

include $(HG_ROOT)/makefile.def

SRC_FILES=$(shell ls *.cpp)
OBJ_FILES=$(subst .cpp,.o,$(SRC_FILES))
DEP_FILES=$(addprefix $(BUILD_DIR)/,$(subst .cpp,.d,$(SRC_FILES)))

TARGETS=libhg.a 

VPATH=$(INC_DIR):$(BUILD_DIR)

all: $(TARGETS) 

-include $(DEP_FILES)

libhg.a: $(OBJ_FILES) 
	$(AR) $(BUILD_DIR)/$@  $(addprefix $(BUILD_DIR)/,$^)

$(BUILD_DIR)/%.d: %.cpp
	@set -e; rm -f $(BUILD_DIR)/$@; \
	$(CC) -MM $(CPPFLAGS) $< | sed 's,\($*\)\.o[ :]*,\1.o $(BUILD_DIR)/$@ : ,g' > $(BUILD_DIR)/$@; \
	echo "	$(CC) $(CFLAGS) $(INC_PATH) -c $< -o $(subst .d,.o,$@) " >> $(BUILD_DIR)/$@


clean:
	$(RM) $(addprefix $(BUILD_DIR)/,$(OBJ_FILES))
	$(RM) $(DEP_FILES)
	$(RM) $(BUILD_DIR)/$(TARGETS)
