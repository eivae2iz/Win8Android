# Если KERNELRELEASE определён, значит вызов сделан из
# системы сборки ядра и можно использовать её язык.
ifneq ($(KERNELRELEASE),)
	obj-m := hello.o
# Иначе вызов сделан прямо из командной
# строки; вызвать систему сборки ядра.
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	#KERNELDIR ?= /home/lesaha/android_x86/kernel/
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm *.o *.ko *.cmd *.order *.symvers *.mod.c -rf .tmp_versions 
endif
