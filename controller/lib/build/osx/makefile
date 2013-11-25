
DEP_DIR = ./dep
BUILD_DIR = ./build
TARGET_DIR = ../../bin
LIB_DIR = ../../src
INCLUDE_DIR = ../../include
OS_DIR = $(LIB_DIR)/osx
PCAP_DIR = /opt/local/include/pcap

TARGET_LIB = $(TARGET_DIR)/avdecc-lib.a

CC := gcc
CXX := g++

INCLUDES += -I$(LIB_DIR)
INCLUDES += -I$(OS_DIR)
INCLUDES += -I$(INCLUDE_DIR)
INCLUDES += -I$(PCAP_DIR)
INCLUDES += -I$(JDKSAVDECC_DIR)/include

EXTRA_CFLAGS += -g
EXTRA_CXXFLAGS += -Wall -ansi -g -std=c++0x

OBJS =  adp.o \
	adp_discovery_state_machine.o \
	acmp.o \
	aecp.o \
	aem_controller_state_machine.o \
	audio_unit_descriptor_imp.o \
	audio_cluster_descriptor_imp.o \
	audio_map_descriptor_imp.o \
	avb_interface_descriptor_imp.o \
	clock_domain_descriptor_imp.o \
	clock_source_descriptor_imp.o \
	configuration_descriptor_imp.o \
	controller_imp.o \
	descriptor_base_imp.o\
	end_station_imp.o \
	entity_descriptor_imp.o \
	jack_input_descriptor_imp.o \
	jack_output_descriptor_imp.o \
	locale_descriptor_imp.o \
	stream_input_descriptor_imp.o \
	stream_output_descriptor_imp.o \
	stream_port_input_descriptor_imp.o \
	stream_port_output_descriptor_imp.o \
	strings_descriptor_imp.o \
	timer.o \
	util_imp.o \
	log.o \
	log_imp.o \
	notification.o \
	notification_imp.o \
	system_message_queue.o \
	system_layer2_multithreaded_callback.o \
	net_interface_imp.o \
	jdksavdecc_pdu.o


# Search .o files in build directory
vpath %.o $(BUILD_DIR)
# Search .c files in the current directory or under other places
vpath %.cpp . $(LIB_DIR) $(OS_DIR)
vpath %.c $(JDKSAVDECC_DIR)/src


all: $(DEP_DIR) $(BUILD_DIR) $(TARGET_LIB)

$(DEP_DIR):
	mkdir -p $(DEP_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

%.o: %.c
	@echo $(CC): $< ...
	@$(CC) $(INCLUDES) $(CFLAGS) $(EXTRA_CFLAGS) -o $(BUILD_DIR)/$(notdir $@) -c $< 

%.o: %.cpp
	@echo $(CXX): $< ...
	@$(CXX) $(INCLUDES) $(CXXFLAGS) $(EXTRA_CXXFLAGS) -o $(BUILD_DIR)/$(notdir $@) -c $< 

$(TARGET_LIB): $(OBJS)
	@echo $(AR): $@ ...
	@$(AR) cr $(TARGET_LIB) $(addprefix $(BUILD_DIR)/,$(notdir $^))

clean:
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


.PHONY: all clean cppcheck

