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
argp.add_argument("-R", "--rvsim", help="Provide the RISCV ISA Sim installation root", type=str)
argp.add_argument("-r", "--rv", help="RISCV binary Sim installation root", type=str, default="/usr/bin")
argp.add_argument("-c", "--clean", action="store_true", help="Remove binaries")
argp.add_argument("-C", "--check", action="store_true", help="Check installations")
argp.add_argument("-b", "--build", action="store_true", help="build the device")
argp.add_argument("-d", "--driver", action="store_true", help="build the driver")
argp.add_argument("-t", "--test", action="store_true", help="build test")

if __name__ == '__main__':
    args = argp.parse_args()
    if args.clean:
        os.system("rm -r out")
    if args.check:
        check_bin_installed()
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
        os.system(f"mkdir -p out; \
                  PATH={os.path.abspath(args.rv)}:$PATH; \
                  riscv64-unknown-elf-gcc test/test_drv.c out/libcrcdriver.o -I driver -o out/crc_test")    