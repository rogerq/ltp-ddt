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
INCLUDES        = -I $(KERNEL_INC) -I src

#List of source files- Update this on adding a new C file
SOURCES := 	src/mc_tvp514.c \
		src/mc_tvp7002.c \
		src/common.c \
		src/parser.c

MAKE_TARGETS            := mc_capture

mc_capture:
	$(CC) $(CFLAGS) $(LDFLAGS) -o mc_capture ${INCLUDES} ${SOURCES}

include $(top_srcdir)/include/mk/generic_leaf_target.mk

