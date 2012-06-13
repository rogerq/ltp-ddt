# MakeFile function     :: MakeFile for compiling EDMA kernel module 

LDFLAGS         += -g -static
INCLUDES        = -I src/user/parser -I src/user/interface/common -I ../utils/user  -I $(KERNEL_DIR)/include
EXTRA_CFLAGS := -I$(src)/../../../utils/kernel
EXTRA_CFLAGS += -I$(src)/interface/common
EXTRA_CFLAGS += -I$(src)/interface/$(PLATFORM)

#List of source files for kernel module - Update this on adding a new C file
SOURCES_KERNEL := \
	testcases/kSt_edma.o \
	testcases/kSt_edma_chain.o \
	testcases/kSt_edma_link.o \
	parser/kSt_edma_parser.o \
	interface/common/kSt_edma_interface.o

obj-m += edma_test.o ../../../utils/kernel/kSt_timer.o
edma_test-objs := $(SOURCES_KERNEL)

MAKE_TARGETS		:= modules

modules:
	$(MAKE) -C $(KERNEL_DIR) ARCH=arm SUBDIRS=$(PWD) modules
	cp edma_test.ko  ../../
	cp ../../../utils/kernel/kSt_timer.ko  ../../


clean:
	rm -f */*.o
	rm -f ../../*.ko
	rm -f interface/common/*.o
	rm -f  *.o
	rm -f  .*.cmd
	rm -rf  .tmp_versions/
	rm -f  *.order
	rm -f  *.c
	rm -f  *.symvers
	rm -f  ./testcases/*.o*
	rm -f  ./testcases/.*.cmd
	rm -f  ./parser/*.o*
	rm -f  ./parser/.*.cmd
	rm -f  ./interface/common/*.o*
	rm -f  ./interface/common/.*.cmd
	rm -f  ../../../utils/kernel/*.ko
	rm -f  ../../../utils/kernel/*.o
	rm -f  ../../../utils/kernel/*.mod.*
	rm -f  ../../../utils/kernel/.*.cmd
	rm -f  *.o
	rm -f *.*
	rm -rf ../../../utils/kernel/.kSt_timer.ko.cmd

