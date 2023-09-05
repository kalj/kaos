#!/usr/bin/env python3

import sys

offset = 0x7C3E
if len(sys.argv) > 1:
    offset = int(sys.argv[1], 0)

for line in sys.stdin:
    if line[7 : (7 + 4)] == "0000":
        addrstr = line[7 : (7 + 8)]
        addr = int(addrstr, 16)
        addr += offset
        newaddrstr = f"{addr:08X}"
        line = line.replace(addrstr, newaddrstr, 1)

    print(line, end="")
