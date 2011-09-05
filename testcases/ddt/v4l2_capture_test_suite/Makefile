# MakeFile function     :: MakeFile for compiling V4L2 Capture test suite 

top_srcdir              ?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile+

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall -O1 -fomit-frame-pointer
LDFLAGS         += -g -static
INCLUDES        = -I $(KERNEL_USR_INC) -I src/interface/common -I ../utils/user

#List of source files- Update this on adding a new C file
SOURCES := \
        src/testcases/st_v4l2_capture_ioctl_test.c \
        src/testcases/st_v4l2_capture_test.c \
        src/interface/common/st_v4l2_capture_common.c \
        ../utils/user/st_cpu_load.c \
        ../utils/user/st_timer.c \
        src/parser/st_v4l2_capture_parser.c

MAKE_TARGETS            := v4l2_capture_tests

v4l2_capture_tests:
	$(CC) $(CFLAGS) $(LDFLAGS) -o v4l2_capture_tests ${INCLUDES} ${SOURCES}

include $(top_srcdir)/include/mk/generic_leaf_target.mk

