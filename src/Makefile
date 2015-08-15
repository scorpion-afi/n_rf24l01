
export ARCH := arm
export CROSS_COMPILE := arm-linux-gnueabihf-

KERNEL_HEADERS := ../../linux_odroid/

all:
	make -C $(KERNEL_HEADERS) M=$(PWD) modules
clean:
	make -C $(KERNEL_HEADERS) M=$(PWD) clean