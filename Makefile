ifndef RISCV
$(error RISCV is unset)
else
$(info Running with RISCV=$(RISCV))
endif

PREFIX ?= $RISCV/
SRC_DIR := src
SRCS=$(wildcard $(SRC_DIR)/*.cc)
INC := $(SRC_DIR)
TARGET ?= libcrcdev
BUILD_DIR = build

default: $(BUILD_DIR)/$(TARGET).so

$(BUILD_DIR)/$(TARGET).so: $(SRCS)
	mkdir -p $(BUILD_DIR)
	g++ -L $(RISCV)/lib -Wl,-rpath,$(RISCV)/lib -shared -o $@ -std=c++17 -I $(INC) -I $(RISCV)/include -fPIC $^

.PHONY: install
install: $(BUILD_DIR)/$(TARGET).so
	cp $(BUILD_DIR)/$(TARGET).so $(RISCV)/lib

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.so