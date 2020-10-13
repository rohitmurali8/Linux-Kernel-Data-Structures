## [M1: point 1]
#  Define MODULE as name of C file which executes function which interact with kernel.
#  ...
MODULE	 = proj2

## [M2: point 1]
#  The Kbuild system will build mod.o file from mod.c file which after linking 
#  would generate the kernel object file.
obj-m += $(MODULE).o

## [M3: point 1]
#  Defines the Kernel source directory.
#  ...
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

## [M4: point 1]
#  Defines the current working directory.
#  ...
PWD := $(shell pwd)

## [M5: point 1]
#  Compile the executable files into bin directory.
#  ...
all: $(MODULE)


## [M6: point 1]
#  Use the C compiler on Linux to generate the .o files 
#  ...
%.o: %.c
	@echo "  CC      $<"
	@$(CC) -c $< -o $@

## [M7: point 1]
#  Compile the code in the current working directory instead of the kernel source directory.
#  ...
$(MODULE):
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

## [M8: point 1]
#  Clean the generated kernel object and compiled files in the current directory.
#  ...
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
