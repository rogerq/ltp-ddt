# MakeFile function     :: MakeFile for compiling RTC test suite 

top_srcdir              ?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall -O1 -fomit-frame-pointer
LDFLAGS         += -g -static
INCLUDES        = -I $(KERNEL_USR_INC) -I $(KERNEL_INC) -I src/interface/common -I ../utils/user

#List of source files- Update this on adding a new C file
SOURCES := \
        ../utils/user/st_fileapi.c \
        src/interface/common/st_rtc_common.c \
        src/parser/st_rtc_parser.c \
        src/testcases/st_rtc_ioctl.c \

MAKE_TARGETS            := rtc_tests

rtc_tests:
	$(CC) $(CFLAGS) $(LDFLAGS) -o rtc_tests ${INCLUDES} ${SOURCES}

include $(top_srcdir)/include/mk/generic_leaf_target.mk

