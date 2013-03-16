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
KERNEL_INCLUDES        = -I $(KERNEL_USR_INC) -I $(KERNEL_INC)  

ifeq ("$(PLATFORM)", "omap3evm")
  CFLAGS += -DCONFIG_OMAP3530
  FILTER_OUT_DIRS += src
  SOURCES :=  	src_omap/mc_tvp514.c \
		src_omap/parser.c \
             	src_omap/common.c
  INCLUDES = $(KERNEL_INCLUDES) -I src_omap
endif

ifeq ("$(PLATFORM)", "am37x-evm")
  CFLAGS += -DCONFIG_OMAP3530
  FILTER_OUT_DIRS += src
  SOURCES :=    src_omap/mc_tvp514.c \
                src_omap/parser.c \
                src_omap/common.c
  INCLUDES = $(KERNEL_INCLUDES) -I src_omap
endif

ifeq ("$(PLATFORM)", "am3517-evm")
  CFLAGS += -DCONFIG_AM3517
  FILTER_OUT_DIRS += src
  SOURCES := src_omap/mc_tvp514.c \
             src_omap/parser.c \
             src_omap/common.c
  INCLUDES = $(KERNEL_INCLUDES) -I src_omap
endif

ifeq ("$(PLATFORM)","dm365-evm")
  FILTER_OUT_DIS += src_omap
  SOURCES :=      src/mc_prv_rsr_tvp514.c \
                src/mc_tvp514.c \
                src/mc_tvp7002.c \
                src/mc_prv_tvp514.c \
                src/common.c \
                src/parser.c
  INCLUDES = $(KERNEL_INCLUDES) -I src
endif



#List of source files- Update this on adding a new C file
#SOURCES := 	src/mc_prv_rsr_tvp514.c \
		src/mc_tvp514.c \
		src/mc_tvp7002.c \
		src/mc_prv_tvp514.c \
		src/common.c \
		src/parser.c \


MAKE_TARGETS            := mc_capture

mc_capture:
	$(CC) $(CFLAGS) $(LDFLAGS) -o mc_capture ${INCLUDES} ${SOURCES} 


include $(top_srcdir)/include/mk/generic_leaf_target.mk

