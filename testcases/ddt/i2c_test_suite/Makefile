# MakeFile function     :: MakeFile for compiling I2C test suite 

top_srcdir              ?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

KERNEL_DIR		:= $(patsubst %/include,%,$(KERNEL_INC))
INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall -O1 -fomit-frame-pointer
LDFLAGS         += -g -static
INCLUDES        = -I $(KERNEL_USR_INC) -I $(KERNEL_INC) -I src/interface/common -I ../utils/user -I $(KERNEL_DIR)/arch/arm/include


#List of source files- Update this on adding a new C file
SOURCES := \
        ../utils/user/st_timer.c \
        ../utils/user/st_cpu_load.c \
        src/testcases/st_i2c_eeprom.c \
        src/interface/common/st_i2c_common.c \
        src/parser/st_i2c_parser.c

MAKE_TARGETS            := i2c_tests

i2c_tests:
	$(CC) $(CFLAGS) $(LDFLAGS) -o i2c_tests ${INCLUDES} ${SOURCES}

include $(top_srcdir)/include/mk/generic_leaf_target.mk

