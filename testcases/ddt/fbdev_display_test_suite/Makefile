# MakeFile function     :: MakeFile for compiling fbdev display test suite 

top_srcdir              ?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall -O1 -fomit-frame-pointer
LDFLAGS         += -g -static -lpthread
INCLUDES        = -I $(KERNEL_USR_INC) -I $(KERNEL_INC) -I src/interface/common -I ../utils/user

#List of source files- Update this on adding a new C file
SOURCES := \
        src/testcases/st_fbdev_ioctl_tests.c \
	src/testcases/st_fbdev_display_tests.c \
	src/testcases/st_fbdev_alphablending_tests.c \
	src/testcases/st_fbdev_colorkey_test.c \
	src/testcases/st_fbdev_pan_display_tests.c \
       src/interface/common/st_fbdev_read_bmp_file.c \
        src/interface/common/st_fbdev_common.c \
        src/interface/common/st_fbdev_interface.c \
        ../utils/user/st_cpu_load.c \
        ../utils/user/st_timer.c \
        src/parser/st_fbdev_parser.c

MAKE_TARGETS            := fbdev_tests

fbdev_tests:
	$(CC) -o fbdev_tests ${INCLUDES} ${SOURCES} ${CFLAGS} ${LDFLAGS}

include $(top_srcdir)/include/mk/generic_leaf_target.mk

