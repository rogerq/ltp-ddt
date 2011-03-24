# MakeFile function     :: MakeFile for compiling Filesystem test suite 

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
INCLUDES        = -I $(KERNEL_INC) -I src/interface/common -I ../utils/user

#List of source files- Update this on adding a new C file
SOURCES := \
        src/testcases/st_filesystem_read_from_file.c \
        src/testcases/st_filesystem_write_to_file.c \
        src/testcases/st_filesystem_copy_file.c \
        src/interface/common/st_filesystem_common.c \
        ../utils/user/st_cpu_load.c \
        ../utils/user/st_timer.c \
        src/parser/st_filesystem_parser.c

MAKE_TARGETS            := filesystem_tests

filesystem_tests:
	$(CC) $(CFLAGS) $(LDFLAGS) -o filesystem_tests ${INCLUDES} ${SOURCES}

include $(top_srcdir)/include/mk/generic_leaf_target.mk

