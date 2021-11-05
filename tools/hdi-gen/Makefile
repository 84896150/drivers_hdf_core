export TARGET:=hdi-gen
export CXXFLAGS = -std=c++14 -O2 -w
export Q := @
export MAKEFLAGS += --no-print-directory

MAKE_FILE_NAME = MakefileLinux
ifeq ($(findstring ;, $(PATH)), ;)
	MAKE_FILE_NAME := MakefileWin
endif

all:
	$(Q) echo start build $(TARGET) ...
	$(Q) $(MAKE) -f $(MAKE_FILE_NAME)

clean:
	$(Q) $(MAKE) -f $(MAKE_FILE_NAME) clean

.PHONY: clean