obj-m := mydev.o

all: x86_build

clean: x86_clean RPI_clean

x86_build: mydev.c
	sudo make -C /lib/modules/5.15.92-v8+/build M=$(shell pwd) modules

x86_clean:
	sudo make -C /lib/modules/5.15.92-v8+/build M=$(shell pwd) clean

RPI_build: mydev.c
	sudo make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C ../linux M=$(shell pwd) modules

RPI_clean:
	sudo make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C ../linux M=$(shell pwd) clean

.PHONY: all clean x86_build x86_clean RPI_build RPI_clean