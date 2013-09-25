
#ARCH := arm
#CROSS_COMPILE := arm-linux-
KVER :=
#KSRC ?= /home/kaspter/System/linux-lts-raring-3.8.0
KSRC ?= /lib/modules/`uname -r`/build
#KSRC ?= /Repos/system/linux




EXTRA_CFLAGS += -I$(KSRC)/drivers/media/dvb-core/
EXTRA_CFLAGS += -I$(KSRC)/drivers/media/tuners/

ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
#obj-m  := mt-demo.o
#mt-demo-y := input-mt-skeleton.o


obj-m := jetxxx.o

jetxxx-y := jetdtv.o jetdvb.o jetfe.o


else
# normal makefile

all: modules

modules:
	$(MAKE) -C $(KSRC) M=$$PWD

#strip:
#	$(CROSS_COMPILE)strip $(MODULE_NAME).ko --strip-unneeded

#install:
#	install -p -m 644

clean:
	rm -rf *.mod.c *.mod *.o .*.cmd *.ko
	rm -rf Module.symvers modules.order
	rm -rf .tmp_versions
endif
