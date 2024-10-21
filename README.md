# crc_spike_dev

To build a simulated hardware peripheral in the (Risc-V) riscv-isa-sim (Spike) and write accompanying target (Risc-V) test to run on that hardware. 
The device shall implement the common binary CRC algorithm for a configurable divisor polynomial. 
The target software shall generate some (pseudo) random test data, and check the CRC calculated by the peripheral against a (reasonably optimized) software implementation.
The Spike peripheral shall contain error checks that validate that the data written to the registers is in a valid state at the moment when the command of execution is given. (if there are invalid, or unreasonable, states)

## Set env and build

```sh
python3 exec.py -h
usage: exec [-h] [-R RVSIM] [-r RV] [-c] [-C] [-b] [-d] [-t] [-g] [-e]

runs bash scripts as per args

options:
  -h, --help            show this help message and exit
  -R RVSIM, --rvsim RVSIM
                        Provide the RISCV ISA Sim installation root
  -r RV, --rv RV        RISCV binary Sim installation root
  -c, --clean           Remove binaries
  -C, --check           Check installations and tools
  -b, --build           build the device
  -d, --driver          build the driver
  -t, --test            build test
  -g, --get_tc          get RiscV toolchaain
  -e, --run_sim         Execute the Simulator
```

### Build Example

The following would get required tools in `tools` directory build them and build the respective target binaries.

```sh
$ python3 build.py -Ccbdt

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
├── device
│   ├── crc_dev.cc
│   └── crc_dev.h
├── driver
│   ├── crc_driver.c
│   └── crc_driver.h
├── exec.py
├── Makefile
├── out
│   ├── compile_commands.json
│   ├── libcrcdev.so
│   ├── libcrcdriver.o
│   ├── libspikedevices.so
│   └── test.dtb
├── README.md
├── test
│   ├── test_drv.c
│   └── test.dts
└── tools
    ├── riscv-isa-sim
    ├── riscv-pk
    ├── risc-v-toolchain
    └── spike-divices
```

## Runnig the test

```sh
python3 exec.py -e
**************Executing simulation***************
tools/riscv-isa-sim/build/install/bin/spike --extlib out/libspikedevices.so --device sifive_uart --extlib out/libcrcdev.so --device crc_dev --dtb=out/test.dtb tools/riscv-pk/build/pk
Found uart at 10000000
Found crc at 20001000
Initialising CRC
*****STARTING Test******
CRC Load: 0x0
CRC Store: 0x0 0x1
CRC Store: 0xc 0x66736628
CRC Load: 0x0
CRC Store: 0x0 0x1
CRC Store: 0x10 0x80018100
CRC Store: 0x14 0x5
hw_crc_convert crc: c216975b
CRC Load: 0x4
CRC Load: 0x8
HW CRC result: 0xc216975b
SW CRC result: 0xc216975b
TEST Result: PASS
*****ENDING Test******
tell me what ELF to load!
```

### Approach to execute

Since, `riscv-pk` is not designed to plugin any new peripheral hence the test execution `fails` if we use the default version of `pk`. 
Managed to hack into the barematal code of `riscv-pk` and added the `test_crc_main()` inside the `machine\minit.c:first_hart_init()` which executes the test before initialising the `MMU`. 

`--extlib out/libcrcdev.so --device crc_dev --dtb=out/test.dtb` is the way to load the dynamic device to `spike` via compiled `dtb` file.

### Known issues or Limitations
* Raising Interrupt is still a TODO
* Random number generation can not be used as we are compiling via `riscv-pk` and it does not support `stdlib`