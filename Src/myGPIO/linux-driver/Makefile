CC = gcc
CC_OPZ = -Wall -Wextra -I..
DEFINE = -D__XIL_GPIO__ -D__DEBUG__
COMPILE = $(CC) $(CC_OPZ) $(DEFINE) -c $< -o $@
LINK = $(CC) $(LINK_OPZ) $^ -o $@
BUILD_N := $(shell date +"%s")

.PHONI: clean all

all: noDriver uio uio-int
	
clean:
	rm -rf *.o noDriver uio uio-int

noDriver : noDriver.o myGPIO.o
	$(LINK)

noDriver.o : noDriver.c
	@echo "BUILD " $(BUILD_N)
	$(COMPILE) -DBUILD=$(BUILD_N)

uio : uio.o myGPIO.o
	$(LINK)

uio.o : uio.c
	@echo "BUILD " $(BUILD_N)
	$(COMPILE) -DBUILD=$(BUILD_N)

uio-int : uio-int.o myGPIO.o xil_gpio.o
	$(LINK)

uio-int.o : uio-int.c
	@echo "BUILD " $(BUILD_N)
	$(COMPILE) -DBUILD=$(BUILD_N)

myGPIO.o : ../myGPIO.c 
	$(COMPILE)

