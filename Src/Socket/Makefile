CC = gcc
CC_OPZ = -Wall -Wextra
DEFINE = 
COMPILE = $(CC) $(CC_OPZ) $(DEFINE) $< -o $@
LINK = $(CC) $(LINK_OPZ) $^ -o $@

all: clientTCP serverTCP

clientTCP : clientTCP.c 
	$(COMPILE)

serverTCP : serverTCP.c
	$(COMPILE)

clean :
	rm -rf clientTCP serverTCP
