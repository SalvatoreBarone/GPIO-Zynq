TARGET = myGPIOK
OBJS = myGPIOK_main.o myGPIOK_t.o myGPIOK_list.o
obj-m += $(TARGET).o
$(TARGET)-y += $(OBJS)
KERNEL_SOURCE ?= $HOME/Linux
PWD := $(shell pwd)
ARCH:=arm
CROSS_COMPILE:=arm-xilinx-linux-gnueabi-

default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean

