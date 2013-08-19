# MakeFile function     :: MakeFile for compiling USB test suite

top_srcdir              ?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall
LDFLAGS         += -g -lpthread

#List of source files- Update this on adding a new C file
TESTUSB_SOURCES	:= src/testcases/testusb.c

UVC_SOURCES	:= src/testcases/usb_video_capture.c

MAKE_TARGETS            := testusb usb_video_capture

testusb: ${TESTUSB_SOURCES}
	 $(CC) $(CFLAGS) $(LDFLAGS) -o testusb  ${TESTUSB_SOURCES}

usb_video_capture: ${UVC_SOURCES}
	 $(CC) $(CFLAGS) $(LDFLAGS) -o usb_video_capture  ${UVC_SOURCES}

include $(top_srcdir)/include/mk/generic_leaf_target.mk
