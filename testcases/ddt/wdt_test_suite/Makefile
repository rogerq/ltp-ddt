# MakeFile function     :: MakeFile for compiling wdt suite 

top_srcdir              ?= ../../..
include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

CFLAGS		= -g -Wall -D__EXPORTED_HEADERS__
LDFLAGS         += -g
INCLUDES        = -I $(KERNEL_INC) -I src/interface/common -I ../utils/user

#Path of header files to be included
INCLUDES        = -I src/parser \
                  -I src/interface/common \
                  -I ../utils/user \
                  -I $(KERNEL_INC)/include \
                  -I $(KERNEL_INC)/arch/arm/include

#List of source files- Update this on adding a new C file
SOURCES := \
        ../utils/user/st_fileapi.c \
        src/interface/common/st_wdt_common.c \
        src/parser/st_wdt_parser.c \
        src/testcases/st_wdt_ioctl.c \
        src/testcases/st_wdt_write.c

MAKE_TARGETS            := wdt_tests

wdt_tests:
	$(CC) $(CFLAGS) $(LDFLAGS) -o wdt_tests ${INCLUDES} ${SOURCES} 

include $(top_srcdir)/include/mk/generic_leaf_target.mk
