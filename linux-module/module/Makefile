obj-m = tcp_ml.o

BUILD_DIR := /lib/modules/$(shell uname -r)/build/
OUTPUT_DIR := $(realpath $(PWD))/build

all:
	make -C $(BUILD_DIR) M=$(PWD) modules
clean:
	make -C $(BUILD_DIR) M=$(PWD) clean
