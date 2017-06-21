obj-m := myGPIOK.o
KERNEL_SOURCE := /home/ssaa/linux-zybo/Linux-Digilent-Dev/
PWD := $(shell pwd)
ARCH=arm
CROSS_COMPILE=arm-xilinx-linux-gnueabi-
CFLAGS_myGPIOK.o := -D__XGPIO__

default:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean

