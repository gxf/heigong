HG_ROOT=..
BUILD_DIR=$(HG_ROOT)/build
INC_DIR=$(HG_ROOT)/inc

include $(HG_ROOT)/makefile.def

TARGETS=hgMaster

LIB_FILES=libprog.a	\
		  libhg.a	\

all: $(TARGETS) 
	$(CP) $^ $(HG_ROOT)/
	$(CP) libhg.a $(HG_ROOT)/

hgMaster: $(LIB_FILES)
	$(LD) $(LDFLAGS) $(LIB_PATH) $(LIBS) $^ -o $@

libprog.a:
	$(MAKE) -C $(HG_ROOT)/prog

libinterface.a:
	$(MAKE) -C $(HG_ROOT)/prog

libhg.a:
	$(MAKE) -C $(HG_ROOT)/lib

clean:
	$(MAKE) clean -C $(HG_ROOT)/prog
	$(MAKE) clean -C $(HG_ROOT)/lib
	$(RM) $(TARGETS)
	$(RM) $(HG_ROOT)/$(TARGETS)
