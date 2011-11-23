# MakeFile function     :: MakeFile for compiling edma test suite 

top_srcdir              ?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall 
LDFLAGS         += -g -static
INCLUDES        = -I src/user/parser -I src/user/interface/common -I ../utils/user  -I $(KERNEL_INC)
EXTRA_CFLAGS := -I$(src)/../utils/kernel
EXTRA_CFLAGS += -I$(src)/src/kernel/interface/common
EXTRA_CFLAGS += -I$(src)/src/kernel/interface/$(PLATFORM)

#List of source files for kernel module - Update this on adding a new C file
SOURCES_KERNEL := \
	src/kernel/testcases/kSt_edma.o \
	src/kernel/testcases/kSt_edma_chain.o \
	src/kernel/testcases/kSt_edma_link.o \
	src/kernel/parser/kSt_edma_parser.o \
	src/kernel/interface/common/kSt_edma_interface.o

obj-m += edma_test.o ../utils/kernel/kSt_timer.o
edma_test-objs := $(SOURCES_KERNEL)

SOURCES_USER := \
	../utils/user/st_fileapi.c \
	../utils/user/st_timer.c \
	../utils/user/st_cpu_load.c \
	src/user/interface/common/st_edma_common.c \
	src/user/parser/st_edma_parser.c \
	src/user/testcases/st_edma_testcase.c

INCLUDES_USER = -I src/user/parser -I src/user/interface/common -I ../utils/user  -I $(KERNEL_DIR)/include

MAKE_TARGETS		:= edma_tests  edma_test.ko kSt_timer.ko

all:
	$(CC) $(CFLAGS) $(LDFLAGS) -o edma_tests ${INCLUDES} ${SOURCES_USER}
	cd src/kernel; make CROSS_COMPILE=$(CROSS_COMPILE)    KERNEL_DIR=$(KERNEL_INC)/.. PLATFORM=$(PLATFORM); cd -

include $(top_srcdir)/include/mk/generic_leaf_target.mk

