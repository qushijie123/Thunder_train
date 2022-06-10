ifneq ($(KERNELRELEASE),)
 	obj-m := kernel_test.o
else
	KERNELDIR ?=/lib/modules/$(shell uname -r)/build
        PWD :=$(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
#        make -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *~ *.o *.ko *.mod.c *.mod *.cmd modules.order Module.markers Module.symvers .tmp_versions
endif
