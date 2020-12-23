# Linux-Kernel-Data-Structures

In this project, implementation of linux modules is the main focus. The implementation of system calls for particular tasks in the linux kernel is a time consuming process, 
which involves building up a new kernel image by executing a series of make commands. 

In order to avoid this, linux kernel data structures are implemented as a Kernel Module. The data structures which are implemented in this project are:
1 - Kernel Linked List
2 - Kernel Hash Table
3 - Kernel Radix Tree
4 - Kernel Red-Black tree
5 - Kernel Xarray

Elements are added to these data structures and the output is seen during the dmesg which is used for Kernel Debugging. 

Ultimately, a bash script is developed in order to execute the module in the kernel
