obj-m += file_varea.o

KDIR := /lib/modules/$(shell uname -r)/build
PVDIR := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PVDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PVDIR) clean

