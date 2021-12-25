LIBS ?= kernel arch esp32_board sec_static utils pm
# WHOLE_LIBS ?= -Wl,--whole-archive -larch -lcmsis -lcore -lcppsupport -lcpup\
# 				   -lexchook -lfatfs -lfs_operations -llitterfs\
# 				   -lmusl-c -lmusl-m -lposix -lsec_static -lshell -lutils\
# 				   -Wl,--no-whole-archive
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/lib $(addprefix -l,$(LIBS))
ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))
COMPONENT_ADD_LINKER_DEPS := $(ALL_LIB_FILES)

