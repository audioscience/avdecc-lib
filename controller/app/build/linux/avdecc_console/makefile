
DEP_DIR = ./dep
BUILD_DIR = ./build
TARGET_DIR = ../../bin
SRC_DIR1 = ../../../cmdline/src
SRC_DIR2 = ../../../test/cmd_line
INCLUDE_LIB_DIR = ../../../../lib/include
AVDECC_LIB = ../../../../lib/bin/avdecc-lib.a


TARGET_APP = $(TARGET_DIR)/avdecc-console

#ARCH := arm

#Compilers
ifeq ($(ARCH),arm)
CC  := arm-none-linux-gnueabi-gcc
CXX  := arm-none-linux-gnueabi-g++
else
CC := gcc
CXX := g++
endif

INCLUDES += -I$(INCLUDE_LIB_DIR)
INCLUDES += -I$(SRC_DIR1)


EXTRA_CFLAGS += -g -D__linux__
EXTRA_CXXFLAGS += -Wall -ansi -g -std=c++0x -D__linux__

OBJS =  cmd_line.o \
	cmd_line_main.o \
	cmd_line_help.o

# Search .o files in build directory
vpath %.o $(BUILD_DIR)
# Search .c files in the current directory or under other places
vpath %.cpp . $(SRC_DIR1) $(SRC_DIR2)


all: $(DEP_DIR) $(BUILD_DIR) $(TARGET_DIR) $(TARGET_APP)

$(DEP_DIR):
	mkdir -p $(DEP_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)


%.o: %.c
	@echo $(CC): $< ...
	@$(CC) $(INCLUDES) $(CFLAGS) -o $(BUILD_DIR)/$(notdir $@) -c $<

%.o: %.cpp
	@echo $(CXX): $< ...
	@$(CXX) $(INCLUDES) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -o $(BUILD_DIR)/$(notdir $@) -c $<

$(AVDECC_LIB):
	@echo make: avdecc-lib
	cd ../../../../lib/build/linux; make

$(TARGET_APP): $(OBJS) $(AVDECC_LIB)
	@echo $(CC) linking $@
	@$(CXX) $(LDFLAGS) -g -lm -lrt -lpthread  -ledit -lcurses -o $@ $(addprefix $(BUILD_DIR)/,$(notdir $(OBJS))) $(AVDECC_LIB)

clean:
	@echo clean: avdecc-lib
	cd ../../../../lib/build/linux; make clean
	rm -f $(OBJS) $(TARGET_LIB) $(TARGET_APP) $(BUILD_DIR)/*.o $(DEP_DIR)/*.d

CPPCHECK_DEPS1 := $(OBJS1:.o=.x)
cppcheck: $(CPPCHECK_DEPS1)

%.x: %.c
	@cppcheck --enable=all $< > $(BUILD_DIR)/$(notdir $*)_cppcheck.log 2>&1
	@cat $(BUILD_DIR)/$(notdir $*)_cppcheck.log
	@grep --color=always -B 0 -A 0 error $(BUILD_DIR)/$(notdir $*)_cppcheck.log || true

%.x: %.cpp
	@cppcheck --enable=all $< > $(BUILD_DIR)/$(notdir $*)_cppcheck.log 2>&1
	@cat $(BUILD_DIR)/$(notdir $*)_cppcheck.log
	@grep --color=always -B 0 -A 0 error $(BUILD_DIR)/$(notdir $*)_cppcheck.log || true


.PHONY: all clean cppcheck $(AVDECC_LIB)

