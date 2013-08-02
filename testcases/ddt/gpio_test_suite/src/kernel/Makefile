# MakeFile function     :: MakeFile for compiling GPIO kernel module 

LDFLAGS         += -g -static
INCLUDES        = -I ../utils/user  -I $(KERNEL_DIR)/include
EXTRA_CFLAGS := -I$(src)/../../../utils/kernel

#List of source files for kernel module - Update this on adding a new C file
SOURCES_KERNEL := \
	testcases/gpio_test.o \

obj-m += gpio_test.o ../../../utils/kernel/kSt_timer.o
gpio_test-objs := $(SOURCES_KERNEL)

MAKE_TARGETS		:= modules

modules:
	$(MAKE) -C $(KERNEL_DIR) ARCH=arm SUBDIRS=$(PWD) modules
	cp gpio_test.ko  ../../
	cp ../../../utils/kernel/kSt_timer.ko  ../../


clean:
	rm -f */*.o
	rm -f ../../*.ko
	rm -f  *.o
	rm -f  .*.cmd
	rm -rf  .tmp_versions/
	rm -f  *.order
	rm -f  *.c
	rm -f  *.symvers
	rm -f  ./testcases/*.o*
	rm -f  ./testcases/.*.cmd
	rm -f  ../../../utils/kernel/*.ko
	rm -f  ../../../utils/kernel/*.o
	rm -f  ../../../utils/kernel/*.mod.*
	rm -f  ../../../utils/kernel/.*.cmd
	rm -f  *.o
	rm -f *.*
	rm -rf ../../../utils/kernel/.kSt_timer.ko.cmd

