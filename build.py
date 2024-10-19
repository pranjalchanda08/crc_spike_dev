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
argp.add_argument("-r", "--riscv", help="Provide the RISCV installation root", type=str)
argp.add_argument("-c", "--check", action="store_true", help="Check installations")
argp.add_argument("-b", "--build", action="store_true", help="build the device")

if __name__ == '__main__':
    args = argp.parse_args()
    if args.check:
        check_bin_installed()
    if args.build:
        os.system(f"mkdir -p out;bear --output out/compile_commands.json -- make RISCV={os.path.abspath(args.riscv)}")
        