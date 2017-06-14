CC = gcc
CC_OPZ = -Wall -Wextra -I../../
DEFINE = -D__XIL_GPIO__
COMPILE = $(CC) $(CC_OPZ) $(DEFINE) -c $< -o $@
LINK = $(CC) $(LINK_OPZ) $^ -o $@
SRC=../../
OUT=uio-int

$(OUT) : $(OUT).o myGPIO.o
	$(LINK)

$(OUT).o : $(OUT).c
	$(COMPILE) -I..

myGPIO.o : $(SRC)myGPIO.c 
	$(COMPILE)

clean :
	rm -rf *.o $(OUT)
