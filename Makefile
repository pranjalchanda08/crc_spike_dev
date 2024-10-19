RISCV ?=
PREFIX ?= $RISCV/
SRC_DIR := device
SRCS=$(wildcard $(SRC_DIR)/*.cc)
INC := $(SRC_DIR)
TARGET ?= libcrcdev
BUILD_DIR = out

default: $(BUILD_DIR)/$(TARGET).so

$(BUILD_DIR)/$(TARGET).so: $(SRCS)
	g++ -L $(RISCV)/lib -Wl,-rpath,$(RISCV)/lib -shared -o $@ -std=c++17 -I $(INC) -I $(RISCV)/include -fPIC $^

.PHONY: install
install: $(BUILD_DIR)/$(TARGET).so
	cp $(BUILD_DIR)/$(TARGET).so $(RISCV)/lib

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.so