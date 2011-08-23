# MakeFile function     :: MakeFile for compiling USB_Video test suite 

top_srcdir              ?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall
LDFLAGS         += -g 

#List of source files- Update this on adding a new C file
SOURCES := \
        src/testcases/usb_video_capture.c \

MAKE_TARGETS            := usb_video_capture

all:
	 $(CC) $(CFLAGS)  -o usb_video_capture  ${SOURCES}


include $(top_srcdir)/include/mk/generic_leaf_target.mk
