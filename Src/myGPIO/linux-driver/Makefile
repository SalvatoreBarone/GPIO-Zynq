CC = gcc
CC_OPZ = -Wall -Wextra -I..
DEFINE = -D__XIL_GPIO__ -D__DEBUG__
COMPILE = $(CC) $(CC_OPZ) $(DEFINE) -c $< -o $@
LINK = $(CC) $(LINK_OPZ) $^ -o $@
.PHONI: clean all


all: noDriver uio uio-int
	
clean:
	rm -rf *.o noDriver uio uio-int

noDriver : noDriver.o myGPIO.o
	$(LINK)

noDriver.o : noDriver.c
	$(COMPILE)

uio : uio.o myGPIO.o
	$(LINK)

uio.o : uio.c
	$(COMPILE)

uio-int : uio-int.o myGPIO.o xil_gpio.o
	$(LINK)

uio-int.o : uio-int.c
	$(COMPILE)

myGPIO.o : ../myGPIO.c 
	$(COMPILE)

