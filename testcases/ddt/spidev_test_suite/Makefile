# MakeFile function     :: MakeFile for compiling SPIDEV test suite 

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
INCLUDES        = -I $(KERNEL_USR_INC) -I $(KERNEL_INC) -I ../utils/user -I $(KERNEL_DIR)/arch/arm/include


#List of source files- Update this on adding a new C file
SOURCES := \
        src/spidev_test.c \

MAKE_TARGETS            := spidev_test

spidev_test:
	$(CC) $(CFLAGS) $(LDFLAGS) -o spidev_test ${INCLUDES} ${SOURCES}

include $(top_srcdir)/include/mk/generic_leaf_target.mk

