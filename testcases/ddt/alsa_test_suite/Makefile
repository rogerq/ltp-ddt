# MakeFile function     :: MakeFile for compiling ALSA Audio test suite 

top_srcdir              ?= ../../..

include $(top_srcdir)/include/mk/env_pre.mk

INSTALL_DIR             := testcases/bin/ddt

FILTER_OUT_DIRS         :=

#Path to ALSA library headers. Change to the path where alsa include folder is present.
ALSA_INCPATH ?= $(KERNEL_USR_INC)

#Path to ALSA library. Change to the path where libasound library is installed.
ALSA_LIBPATH ?= $(KERNEL_USR_INC)/../lib


# We don't want to copy over the Makefile
UNWANTED_FILES          := Makefile

INSTALL_MODE            := 00755

#INSTALL_TARGETS         := $(filter-out $(UNWANTED_FILES),$(notdir $(patsubst $(abs_srcdir)/%,%,$(wildcard $(abs_srcdir)/*))))

CFLAGS          += -g -Wall -O1 -fomit-frame-pointer
LDFLAGS         += -g 
INCLUDES        = -I $(KERNEL_USR_INC) -I $(KERNEL_INC) -I src/interface/common -I ../utils/user -I $(ALSA_INCPATH)

#List of source files- Update this on adding a new C file
SOURCES := \
        ../utils/user/st_fileapi.c \
        ../utils/user/st_timer.c \
        ../utils/user/st_cpu_load.c \
        src/testcases/st_alsa_capture_test.c \
        src/testcases/st_alsa_playback_test.c \
        src/testcases/st_alsa_file_playback_test.c\
        src/testcases/st_alsa_capture_file_test.c\
        src/interface/common/st_alsa_interface.c \
        src/parser/st_alsa_parser.c

MAKE_TARGETS            := alsa_perf_tests

alsa_perf_tests:
	$(CC) $(CFLAGS) $(LDFLAGS) -o alsa_perf_tests ${INCLUDES} ${SOURCES} -lasound -lm -L$(ALSA_LIBPATH)


include $(top_srcdir)/include/mk/generic_leaf_target.mk

