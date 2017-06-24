TARGET = myGPIOK
OBJS = myGPIOK_main.o myGPIOK_t.o myGPIOK_dev_int.o


obj-m += $(TARGET).o
$(TARGET)-y += $(OBJS)
KERNEL_SOURCE := /home/ssaa/linux-zybo/Linux-Digilent-Dev/
PWD := $(shell pwd)
BUILD_N := $(shell date +"%s")
ARCH=arm
CROSS_COMPILE=arm-xilinx-linux-gnueabi-
CFLAGS_myGPIOK.o := -DBUILD=$(BUILD_N)

default:
	@echo "BUILD ID : " $(BUILD_N)
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} modules
	
clean:
	${MAKE} -C ${KERNEL_SOURCE} SUBDIRS=${PWD} clean

