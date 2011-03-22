# MakeFile function     :: MakeFile for compiling Filesystem test suite 

#USERDEFINES should be included first for the user environment variables.
-include ../USERDEFINES

CC              = $(TOOL_PREFIX)gcc 

#Flags for compilation
CFLAGS          = -g -Wall -O1 -fomit-frame-pointer

#LD flags
LDFLAGS         = -g -static

#Installation directory - Change based on desired path for placing test suite executable
INSTDIR         = bin

#Path of header files to be included
INCLUDES        = -I $(KERNEL_DIR)/include -I src/interface/common -I ../utils/user

#List of source files- Update this on adding a new C file
SOURCES := \
        src/testcases/st_filesystem_read_from_file.c \
        src/testcases/st_filesystem_write_to_file.c \
        src/testcases/st_filesystem_copy_file.c \
        src/interface/common/st_filesystem_common.c \
	../utils/user/st_cpu_load.c \
	../utils/user/st_timer.c \
        src/parser/st_filesystem_parser.c

#Rule to build
all:
	$(CC) $(CFLAGS) $(LDFLAGS) -o filesystem_tests $(INCLUDES) $(SOURCES) 
	mkdir -p $(INSTDIR)/$(PLATFORM)
	mv filesystem_tests $(INSTDIR)/$(PLATFORM)

#Rule to clean
clean:
	rm -f  *.o
	rm -rf  ./$(INSTDIR)/$(PLATFORM) 

