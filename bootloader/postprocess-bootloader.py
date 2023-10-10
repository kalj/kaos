#!/usr/bin/env python3

import sys, pathlib

MBR_LOAD_ADDRESS = 0x7C00

assert len(sys.argv) == 4

bin_path = pathlib.Path(sys.argv[1])
code_offset = int(sys.argv[2])
lst_path = pathlib.Path(sys.argv[3])

# rewrite lst file
code_address = MBR_LOAD_ADDRESS + code_offset

input_lines = open(lst_path, "r").read().splitlines()
output_lines = []

for line in input_lines:
    if line[7 : (7 + 4)] == "0000":
        addrstr = line[7 : (7 + 8)]
        addr = int(addrstr, 16)
        addr += code_address
        newaddrstr = f"{addr:08X}"
        line = line.replace(addrstr, newaddrstr, 1)

    output_lines.append(line)

open(lst_path, "w").write("\n".join(output_lines))

# compute size
size = bin_path.stat().st_size

slack = 512 - 2 - code_offset - size

if slack >= 0:
    print("-- Spare space in bootloader:", slack)
else:
    print(f"-- Bootloader size overflow, {-slack} B too large!")
    sys.exit(1)
