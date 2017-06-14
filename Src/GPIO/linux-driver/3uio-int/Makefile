CC = gcc
CC_OPZ = -Wall -Wextra -I. -I.. -I../../
DEFINE = -D__XIL_GPIO__ -D__DEBUG__
COMPILE = $(CC) $(CC_OPZ) $(DEFINE) -c $< -o $@
LINK = $(CC) $(LINK_OPZ) $^ -o $@
SRC=../
OUT=uio-int

$(OUT) : $(OUT).o myGPIO.o xil_gpio.o
	$(LINK)

$(OUT).o : $(OUT).c
	$(COMPILE)

myGPIO.o : $(SRC)myGPIO.c 
	$(COMPILE)
	
xil_gpio.o : xil_gpio.c xil_gpio.h 
	$(COMPILE)
	
clean :
	rm -rf *.o $(OUT)
