import os
import argparse
import shutil

dependent_prog = [
    ('bear', 'bear'),
    ('make', 'make'),
    ('g++', 'build-essential'),
    ('gcc', 'build-essential')
]

def check_bin_installed():
    for bin in dependent_prog:
        path = shutil.which(bin[0])
        if path is None:
            print(f"Installing {bin[0]}")
            os.system(f"sudo apt-get install {bin[1]} -y")
        else:
            print(f"{bin[0]} installed: {path}") 

argp = argparse.ArgumentParser(
                    prog='exec',
                    description='runs bash scripts as per args')
argp.add_argument("-R", "--rvsim", help="Provide the RISCV ISA Sim installation root", type=str, default="tools/riscv-isa-sim/build/install")
argp.add_argument("-r", "--rv", help="RISCV binary Sim installation root", type=str, default="tools/risc-v-toolchain/bin")
argp.add_argument("-c", "--clean", action="store_true", help="Remove binaries")
argp.add_argument("-C", "--check", action="store_true", help="Check installations and tools")
argp.add_argument("-b", "--build", action="store_true", help="build the device")
argp.add_argument("-d", "--driver", action="store_true", help="build the driver")
argp.add_argument("-t", "--test", action="store_true", help="build test")
argp.add_argument("-g", "--get_tc", action="store_true", help="get RiscV toolchaain")
argp.add_argument("-e", "--run_sim", action="store_true", help="Execute the Simulator")

if __name__ == '__main__':
    args = argp.parse_args()
    if args.clean:
        os.system("rm -r out")
    if args.get_tc:
        os.system( "git clone https://github.com/VisorFolks/risc-v-toolchain.git tools/risc-v-toolchain")
    if args.check:
        check_bin_installed()
        os.system("git clone -q -b crc_device_test https://github.com/pranjalchanda08/riscv-pk tools/riscv-pk;")
        os.system("git clone -b https://github.com/pranjalchanda08/riscv-isa-sim tools/riscv-isa-sim;" \
                    "cd tools/riscv-isa-sim;"
                    "bash ci-tests/build-spike")
        os.system("git clone https://github.com/ucb-bar/spike-devices.git tools/spike-divices;" \
            "cd tools/spike-divices;" \
            f"export RISCV={args.rvsim};" \
            "make -j8;cp libspikedevices.so ../../out/")
        
    if args.build:
        print(f"Building Device plugin from: ./device")
        os.system(f"mkdir -p out;bear --output out/compile_commands.json -- make RISCV={os.path.abspath(args.rvsim)}")
    if args.driver:
        print(f"Building Driver from: ./driver")
        os.system(f"mkdir -p out; \
                  PATH={os.path.abspath(args.rv)}:$PATH; \
                  riscv64-unknown-elf-gcc -c driver/crc_driver.c -I driver -o out/libcrcdriver.o")
    if args.test:
        print(f"Building test from: ./test")
        os.system("mkdir -p out;dtc -I dts -O dtb test/test.dts > out/test.dtb")
        os.system("cd tools/spike-divices;" \
            f"export RISCV={args.rvsim};" \
            "make -j8;cp libspikedevices.so ../../out/")
        os.system(  f"cp driver/* test/*.c tools/riscv-pk/machine/.;" \
                    "cd tools/riscv-pk;" \
                    "mkdir -p build; cd build;" \
                    f"PATH={os.path.abspath(args.rv)}:$PATH;" \
                    f"../configure --prefix={os.path.abspath(args.rv)} --host=riscv64-unknown-elf;" \
                    "make -j8")
    if args.run_sim:
        print(f"**************Executing simulation***************")
        print(f"{args.rvsim}/bin/spike --extlib out/libspikedevices.so --device sifive_uart --extlib out/libcrcdev.so --device crc_dev --dtb=out/test.dtb tools/riscv-pk/build/pk")
        os.system(f"{os.path.abspath(args.rvsim)}/bin/spike --extlib out/libspikedevices.so --device sifive_uart --extlib out/libcrcdev.so --device crc_dev --dtb=out/test.dtb tools/riscv-pk/build/pk")
        