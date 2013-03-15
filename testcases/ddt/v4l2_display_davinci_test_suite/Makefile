# MakeFile function     :: MakeFile for compiling Filesystem test suite 

top_srcdir              ?= ../../..

CMEMLIB=src/cmem.a470uC
include $(top_srcdir)/include/mk/env_pre.mk
INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall -O1 -fomit-frame-pointer
LDFLAGS         += -g -static
INCLUDES        = -I $(KERNEL_USR_INC) -I $(KERNEL_INC) 

#List of source files- Update this on adding a new C file
SOURCES := 	v4l2_colors.c 

MAKE_TARGETS            := davinci_v4l2_display

davinci_v4l2_display:
	$(CC) $(CFLAGS) $(LDFLAGS) -o davinci_v4l2_display ${INCLUDES} ${SOURCES} 


include $(top_srcdir)/include/mk/generic_leaf_target.mk

