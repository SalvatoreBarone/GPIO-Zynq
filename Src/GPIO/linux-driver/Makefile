.PHONI: clean all copy unmount
destination=/media/ssaa/rootfs/root/myDriver/linux-driver/
devices=/media/ssaa/*

drivers=1noDriver/ 2uio/ 3uio-int/

all:
	for i in $(drivers); do \
		make -C $$i; \
	done; \
	
	
clean:
	for i in $(drivers); do \
		make clean -C $$i; \
	done;
