# crc_spike_dev

To build a simulated hardware peripheral in the (Risc-V) riscv-isa-sim (Spike) and write accompanying target (Risc-V) test to run on that hardware. 
The device shall implement the common binary CRC algorithm for a configurable divisor polynomial. 
The target software shall generate some (pseudo) random test data, and check the CRC calculated by the peripheral against a (reasonably optimized) software implementation.
The Spike peripheral shall contain error checks that validate that the data written to the registers is in a valid state at the moment when the command of execution is given. (if there are invalid, or unreasonable, states)

## Set env and build

```sh
$ python3 build.py -h
```
```sh
python3 build.py -h
usage: exec [-h] [-R RVSIM] [-r RV] [-c] [-C] [-b] [-d] [-t]

runs bash scripts as per args

options:
  -h, --help            show this help message and exit
  -R RVSIM, --rvsim RVSIM
                        Provide the RISCV ISA Sim installation root
  -r RV, --rv RV        RISCV binary Sim installation root
  -c, --clean           Remove binaries
  -C, --check           Check installations
  -b, --build           build the device
  -d, --driver          build the driver
  -t, --test            build test
```

### Build Example
```sh
$ python3 build.py -R ../riscv-isa-sim/build/install -r ../risc-v-toolchain/bin -Ccbdt

bear installed: /usr/bin/bear
make installed: /usr/bin/make
g++ installed: /usr/bin/g++
gcc installed: /usr/bin/gcc
Building Device plugin from: ./device
Building Driver from: ./driver
Building test from: ./test
```

## File Tree

```sh
├── build.py
├── device
│   ├── crc_dev.cc
│   └── crc_dev.h
├── driver
│   ├── crc_driver.c
│   └── crc_driver.h
├── Makefile
├── out
│   ├── compile_commands.json
│   ├── crc_test
│   ├── libcrcdev.so
│   ├── libcrcdriver.o
│   └── test.dtb
├── README.md
└── test
    ├── test_drv.c
    └── test.dts
```

## Runnig the test

```sh

```