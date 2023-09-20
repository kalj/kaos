#!/bin/bash

image=build/main.img

if [ -ge $# 1 ] ; then
    image="$1"
fi

gdb -ex 'set disassembly-flavor intel' \
    -ex "target remote | qemu-system-i386 -S -gdb stdio -m 32 -fda ${image}" \
    -ex 'layout asm' \
    -ex 'layout reg' \
    -ex 'set architecture i386' \
    -ex 'break *0x7c00' 

