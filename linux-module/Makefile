obj-m = tcp_ml.o

BUILD_DIR := /lib/modules/$(shell uname -r)/build/
OUTPUT_DIR := $(PWD)

module: tcp_ml.c
	make -C $(BUILD_DIR) M=$(PWD) modules

driver: tcp_ml_driver.cc
	g++ -o driver tcp_ml_driver.cc

all: module driver

clean:
	make -C $(BUILD_DIR) M=$(PWD) clean
	rm driver
