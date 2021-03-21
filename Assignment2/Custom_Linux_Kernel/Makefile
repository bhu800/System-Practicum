KERNELDIR = /lib/modules/5.10.1/build
MODULES = try.ko
obj-m +=try.o

all:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
install:
	make -C $(KERNELDIR) M=$(PWD) modules_install
